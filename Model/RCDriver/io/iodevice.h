/**
 * @file iodevice.h
 * @brief Объявление интерфейса устройства ввода/вывода
 */
#pragma once

#include <cstddef>
#include <memory>

#include "listeners/listenermanager.h"
#include "connection.h"
#include "message.h"

/**
 * @class IODevice
 * @brief Интерфейс устройства ввода/вывода
 * @details Интерфейс предполагает отправление сообщений с данными через метод,
 *          получение сообщений через паттерн наблюдателя
 */
class IODevice : public ListenerManager {
public:
    virtual ~IODevice() = default;

    /**
     * @brief Уточнить максимальный размер массива передаваемых данных одним
     *        сообщением, поддерживаемый устройством
     * @return Максимальный размер массива данных
     */
    virtual std::size_t dataLimit() const noexcept = 0;

    /**
     * @brief Начать информационный обмен через устройство
     * @return true, если информационный обмен был успешно начат
     */
    virtual bool start() = 0;
    /**
     * @brief Прекратить информационный обмен через устройство
     */
    virtual void stop() = 0;

    /**
     * @brief Получить информацию о состоянии соединения устройства
     * @return Текущее состояние устройства
     */
    virtual ConnectionStatus status() const noexcept = 0;

    /**
     * @brief Отправить сообщение на устройство
     * @param outgoing Сообщение, которое необходимо отправить
     */
    virtual void write(ConstMessage outgoing) = 0;

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

using IODevice_Ptr = std::shared_ptr<IODevice>;
using IODevice_WeakPtr = std::weak_ptr<IODevice>;
