/**
 * @file protocol.h
 * @brief Объявление интерфейса протокола информационного взаимодействия
 */
#pragma once

#include <memory>

#include "io/iodevice.h"
#include "io/message.h"

/**
 * @class Protocol
 * @brief Интерфейс протокола информационного взаимодействия
 * @details Протокол является оберткой для устройства ввода-вывода, реализующий
 *          распаковку входящих сообщений и упаковку исходящих согласно
 *          протоколу информационного взаимодействия. \n
 *
 *          Реализация протокола должна поддерживать следующие типы
 *          наблюдателей: MessageListener.
 */
class Protocol : public IODevice {
public:
    virtual ~Protocol() = default;

    /**
     * @brief Задать устройство, через которое осуществляется информационный
     *        обмен
     * @param device Устройство ввода/вывода
     */
    virtual void setDevice(IODevice_Ptr device) = 0;

    /**
     * @brief Начать информационный обмен
     * @return true, если информационный обмен был успешно запущен
     */
    virtual bool start() override = 0;
    /**
     * @brief Прекратить информационный обмен
     */
    virtual void stop() override = 0;

    /**
     * @brief Уточнить максимальный размер массива передаваемых данных одним
     *        сообщением, поддерживаемый протоколом
     * @return Максимальный размер массива данных
     */
    virtual std::size_t dataLimit() const noexcept override = 0;

    /**
     * @brief Получить информацию о состоянии устройства, по которому
     *        осуществляется информационный обмен с применением
     *        протокола взаимодействия
     * @return Текущее состояние устройства
     */
    virtual ConnectionStatus status() const noexcept override = 0;

    /**
     * @brief Сериализовать и отправить данные сообщения с применением
     *        информационного протокола
     * @param msg Сообщение для отправки
     */
    virtual void write(ConstMessage msg) override = 0;

    /**
     * @brief Привязать наблюдателя
     * @details Реализация интерфейса должна поддерживать следующие типы
     *          наблюдателей: MessageListener
     * @param listener Новый наблюдатель
     */
    virtual void attach(Listener_Ptr listener) override = 0;
    /**
     * @brief Отвязать наблюдателя
     * @param listener Наблюдатель, которого необходимо отвязать
     */
    virtual void detach(Listener_Ptr listener) override = 0;
};

using Protocol_Ptr = std::shared_ptr<Protocol>;
using Protocol_WeakPtr = std::weak_ptr<Protocol>;
