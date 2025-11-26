/**
 * @file serialprotocol.h
 * @brief Объявление класса SerialProtocol
 */
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

#include "io/iodevice.h"
#include "io/message.h"
#include "listeners/listener.h"
#include "listeners/messagelistener.h"
#include "listeners/notifier.h"
#include "protocol.h"

/**
 * @class SerialProtocol
 * @brief Реализация протокола информационного обмена для последовательного
 *        порта
 * @details Данный информационный протокол является универсальным протоколам
 *          информационного взаимодействия по последовательным портам (USB,
 *          UART и т.д.). Главной задачей протокола является обеспечение
 *          синхронизации и безопасности передаваемых данных. \n
 *
 *          Каждое передаваемое сообщение данных упаковывается в пакет.
 *          Пакет данных выглядит следующим образом: \n
 *
 *          | НОМЕР БАЙТА | ДИАП. ЗНАЧЕНИЙ | ОПИСАНИЕ                       |
 *          | ----------- | -------------- | ------------------------------ |
 *          | [0, 1]      | 0x35, 0xCA     | Фиксированный заголовок пакета |
 *          | [2]         | [0, 255]       | Размер полезных данных, N      |
 *          | [3, 3+N)    | [0, 255]       | Полезные данные                |
 *          | [3+N]       | [0, 255]       | CRC8 полезных данных           |
 */
class SerialProtocol
    : public Protocol
    , public MessageListener
    , public std::enable_shared_from_this<SerialProtocol> {

public:
    /**
     * @brief Максимальный размер полезных данных, передаваемых одним сообщением
     */
    static constexpr uint8_t k_MaxPayloadSize = 255;

    SerialProtocol();

    /**
     * @brief Задать устройство, через которое осуществляется информационный
     *        обмен
     * @param device Устройство ввода/вывода
     */
    void setDevice(IODevice_Ptr device) override;

    /**
     * @brief Начать информационный обмен
     * @details Протокол только начинает отслеживать данные, приходящие на порт.
     *          Инициализацию и начало работы порта пользователь реализует
     *          самостоятельно.
     * @return true, если информационный обмен был успешно запущен
     */
    bool start() override;
    /**
     * @brief Прекратить информационный обмен
     * @warning После остановки необходимо заново передать указатель на
     *          устройство в метод \ref setDevice
     */
    void stop() override;

    /**
     * @brief Уточнить максимальный размер массива передаваемых данных одним
     *        сообщением, поддерживаемый протоколом
     * @return Максимальный размер массива данных
     */
    std::size_t dataLimit() const noexcept override { return k_MaxPayloadSize; }

    /**
     * @brief Получить информацию о состоянии устройства, по которому
     *        осуществляется информационный обмен с применением
     *        протокола взаимодействия
     * @return Текущее состояние устройства
     */
    ConnectionStatus status() const noexcept override;

    /**
     * @brief Сериализовать данные сообщения и отправить в порт
     * @param msg Сообщение для отправки
     */
    void write(ConstMessage msg) override;

    /**
     * @brief Привязать наблюдателя
     * @details Протокол поддерживает следующие типы наблюдателей:
     *          MessageListener
     * @param lisetener Новый наблюдатель
     */
    void attach(Listener_Ptr listener) override;
    /**
     * @brief Отвязать наблюдателя
     * @param lisetener Наблюдатель, которого необходимо отвязать
     */
    void detach(Listener_Ptr listener) override;

    /**
     * @brief Обработать входящее сообщение
     * @param status Данные нового входящего сообщения
     */
    void messageReceived(ConstMessage message) override;

protected:
    /**
     * @brief Обработать входящий байт
     * @param byte Очередной байт входящего сообщения
     * @return true, если очередной байт не нарушает структуру пакета протокола
     *         информационного обмена
     */
    bool handle(uint8_t byte) noexcept;

private:
    IODevice_WeakPtr m_weakDevice;
    std::atomic_bool m_started = false;

    Message m_rxBuff;
    ListenerNotifier m_notifier;
};
