/**
 * @file serialport.cpp
 * @brief Определение класса SerialPort
 */
#include "serialport.h"

#include <cassert>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "listeners/connectionlistener.h"
#include "listeners/messagelistener.h"
#include "context.h"

SerialPort::SerialPort()
    : m_port(static_cast<boost::asio::io_context &>(Context::get())) {}

bool SerialPort::start() {
    if (m_subjects.connection == ConnectionStatus::k_Connected ||
        (m_id.empty() || !m_baudrate)) {

        return false;
    }

    boost::system::error_code ec;
    ec = m_port.open(m_id, ec);

    if (ec) {
        return false;
    }

    m_subjects.connection = ConnectionStatus::k_Connected;
    m_notifiers.connection.notify(m_subjects.connection);

    m_port.set_option(boost::asio::serial_port_base::baud_rate(m_baudrate));
    boost::asio::co_spawn(
            static_cast<boost::asio::io_context &>(Context::get()),
            asyncRead(), boost::asio::detached);

    return true;
}

void SerialPort::stop() {
    if (m_subjects.connection != ConnectionStatus::k_Connected) {
        return;
    }

    boost::system::error_code ec;
    // Костыль, чтобы подавить предупреждение
    [[maybe_unused]] auto _ = m_port.close(ec);

    m_subjects.connection = ConnectionStatus::k_Disconnected;
    m_notifiers.connection.notify(m_subjects.connection);
}

ConnectionStatus SerialPort::status() const noexcept {
    return m_subjects.connection;
}

void SerialPort::write(ConstMessage msg) {
    if (m_subjects.connection != ConnectionStatus::k_Connected) {
        return;
    }

    assert(msg->size() <= k_RxBuffSize);

    std::lock_guard lock(m_txQueue_mtx);
    m_txQueue.push(std::move(msg));

    if (m_txQueue.size() == 1) {
        boost::asio::co_spawn(
            static_cast<boost::asio::io_context &>(Context::get()),
            asyncWrite(), boost::asio::detached);
    }
}

void SerialPort::attach(Listener_Ptr listener) {
    if (dynamic_cast<MessageListener *>(listener.get())) {
        m_notifiers.message.attach(listener);
    }
    else if (dynamic_cast<ConnectionListener *>(listener.get())) {
        m_notifiers.connection.attach(listener);
    }
}

void SerialPort::detach(Listener_Ptr listener) {
    if (dynamic_cast<MessageListener *>(listener.get())) {
        m_notifiers.message.detach(listener);
    }
    else if (dynamic_cast<ConnectionListener *>(listener.get())) {
        m_notifiers.connection.detach(listener);
    }
}

boost::asio::awaitable<void> SerialPort::asyncRead() {
    for (;;) {
        m_subjects.message = Message::create(k_RxBuffSize);

        auto [ec, len] = co_await m_port.async_read_some(
            boost::asio::buffer(**m_subjects.message),
            boost::asio::as_tuple(boost::asio::use_awaitable));

        if (ec) {
            break;
        }

        (*m_subjects.message)->resize(len);
        m_notifiers.message.notify(m_subjects.message);
    }

    stop();
}

boost::asio::awaitable<void> SerialPort::asyncWrite() {
    for (;;) {
        auto ec = co_await m_port.async_write_some(
            boost::asio::buffer(*m_txQueue.front()),
            boost::asio::use_awaitable);

        std::lock_guard lock(m_txQueue_mtx);
        m_txQueue.pop();

        if (ec) {
            std::queue<ConstMessage> empty;
            m_txQueue.swap(empty);
        }

        if (m_txQueue.empty()) {
            break;
        }
    }
}
