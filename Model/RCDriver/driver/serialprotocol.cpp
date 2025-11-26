/**
 * @file serialprotocol.cpp
 * @brief Определение класса SerialProtocol
 */
#include "serialprotocol.h"

#include <cassert>
#include <cstdint>
#include <cstring>

#include "common/crc8.h"
#include "io/connection.h"
#include "listeners/subjectcontainer.h"

enum : uint8_t {
    k_Header0_Pos  = 0,
    k_Header0_Data = 0x35,

    k_Header1_Pos  = 1,
    k_Header1_Data = 0xCA,

    k_PayloadSize_Pos = 2,
    k_PayloadData_Pos = 3,

    k_ExtraPackageBytes = 4
};

SerialProtocol::SerialProtocol() : m_rxBuff(Message::create()) {
    m_rxBuff->reserve(k_MaxPayloadSize);
}

void SerialProtocol::setDevice(IODevice_Ptr device) {
    if (m_started) {
        return;
    }

    m_weakDevice = device;
}

bool SerialProtocol::start() {
    auto device = m_weakDevice.lock();
    if (m_started || !device || device->dataLimit() < k_MaxPayloadSize) {
        return false;
    }

    m_started = true;
    device->attach(shared_from_this());

    return true;
}

void SerialProtocol::stop() {
    auto device = m_weakDevice.lock();
    if (!device || !m_started) {
        return;
    }

    m_started = false;
    device->detach(shared_from_this());
    // m_weakDevice.reset();

    m_rxBuff->clear();
}

ConnectionStatus SerialProtocol::status() const noexcept {
    auto device = m_weakDevice.lock();
    if (!device) {
        return ConnectionStatus::k_Undefined;
    }

    return device->status();
}

void SerialProtocol::write(ConstMessage msg) {
    auto device = m_weakDevice.lock();
    if (!device) {
        return;
    }

    assert(msg->size() <= k_MaxPayloadSize);
    assert(msg->size() >= 1);

    Message outgoing = Message::create(msg->size() + k_ExtraPackageBytes);
    MessageData &ref = *outgoing;

    ref[k_Header0_Pos] = k_Header0_Data;
    ref[k_Header1_Pos] = k_Header1_Data;
    ref[k_PayloadSize_Pos] = static_cast<uint8_t>(msg->size());
    std::memcpy(&ref[k_PayloadData_Pos], msg->data(), msg->size());
    ref.back() = crc8(msg->data(), msg->size());

    device->write(std::move(outgoing));
}

void SerialProtocol::attach(Listener_Ptr listener) {
    if (dynamic_cast<MessageListener *>(listener.get())) {
        m_notifier.attach(listener);
    }
}

void SerialProtocol::detach(Listener_Ptr listener) {
    if (dynamic_cast<MessageListener *>(listener.get())) {
        m_notifier.detach(listener);
    }
}

void SerialProtocol::messageReceived(ConstMessage message) {
    for (uint8_t byte : *message) {
        handle(byte);
    }
}

bool SerialProtocol::handle(uint8_t byte) noexcept {
    MessageData &ref = *m_rxBuff;

    if ((ref.size() == k_Header0_Pos && byte != k_Header0_Data) ||
        (ref.size() == k_Header1_Pos && byte != k_Header1_Data)) {

        ref.clear();
        return false;
    }

    ref.push_back(byte);
    if ((ref.size() < k_PayloadData_Pos) ||
        (static_cast<uint8_t>(ref.size()) < ref[k_PayloadSize_Pos] + k_ExtraPackageBytes)) {

        return true;
    }

    uint8_t checksum = crc8(&ref[k_PayloadData_Pos], ref[k_PayloadSize_Pos]);
    if (ref.back() != checksum) {
        ref.clear();
        return false;
    }

    SubjectContainer<Message> payload = Message::create(ref[k_PayloadSize_Pos]);
    std::memcpy((*payload)->data(), &ref[k_PayloadData_Pos], (*payload)->size());

    m_notifier.notify(std::move(payload));
    ref.clear();

    return true;
}
