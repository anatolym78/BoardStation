/**
 * @file serialport.h
 * @brief Объявление класса SerialPort
 */
#pragma once

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>

#include "listeners/notifier.h"
#include "listeners/subjectcontainer.h"
#include "connection.h"
#include "message.h"
#include "serial.h"

/**
 * @class SerialPort
 * @brief Реализация интерфейса последовательного порта
 */
class SerialPort : public Serial {
public:
    SerialPort();

    /**
     * @brief Задать идентификатор последовательного порта
     * @param port Идентификатор последовательного порта
     */
    void setPort(std::string port) override { m_id = port; }
    /**
     * @brief Задать скорость передачи данных
     * @param baudrate Необходимая скорость передачи данных
     */
    void setBaudrate(uint32_t baudrate) override { m_baudrate = baudrate; }

    /**
     * @brief Начать работу последовательного порта
     * @details Метод открывает последовательный порт и осуществляет
     *          регистрацию асинхронных операций в контексте.
     *          Для начала выполнения этих операций необходимо самостоятельно
     *          запустить контекст, если он не был запущен до этого.
     * @return false, если не удалось инициализировать порт
     */
    bool start() override;
    /**
     * @brief Завершить работу последовательного порта
     */
    void stop() override;

    /**
     * @brief Уточнить максимальный размер массива передаваемых данных одним
     *        сообщением, поддерживаемый портом
     * @return Максимальный размер массива данных
     */
    std::size_t dataLimit() const noexcept override { return k_RxBuffSize; }

    /**
     * @brief Получить информацию о состоянии соединения порта
     * @return Текущее состояние порта
     */
    ConnectionStatus status() const noexcept override;

    /**
     * @brief Отправить сообщение в порт
     * @param msg Сообщение для отправки
     */
    void write(ConstMessage msg) override;

    /**
     * @brief Привязать наблюдателя
     * @details SerialPort поддерживает следующие типы наблюдателей:
     *          ConnectionListener, MessageListener
     * @param listener Новый наблюдатель
     */
    void attach(Listener_Ptr listener) override;
    /**
     * @brief Отвязать наблюдателя
     * @param listener Наблюдатель, которого необходимо отвязать
     */
    void detach(Listener_Ptr listener) override;

private:
    /**
     * @brief Начать асинхронный цикл чтения
     */
    boost::asio::awaitable<void> asyncRead();
    /**
     * @brief Начать асинхронный цикл записи
     */
    boost::asio::awaitable<void> asyncWrite();

    enum {
        k_RxBuffSize = 1024
    };

    boost::asio::serial_port m_port;

    std::string m_id;
    uint32_t m_baudrate = 115200;

    std::queue<ConstMessage> m_txQueue;
    std::mutex m_txQueue_mtx;

    struct {
        ListenerNotifier connection;
        ListenerNotifier message;
    } m_notifiers;
    struct {
        SubjectContainer<ConnectionStatus> connection =
            ConnectionStatus::k_Undefined;
        SubjectContainer<Message> message;
    } m_subjects;
};
