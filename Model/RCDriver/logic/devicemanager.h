/**
 * @file devicemanager.h
 * @brief Объявление класса DeviceManager
 */
#pragma once

#include <map>
#include <string>

#include "driver/protocol.h"
#include "io/iodevice.h"
#include "manager.h"

/**
 * @class DeviceManager
 * @brief Диспетчер устройств ввода/вывода
 * @details Диспетчер устройств хранит объекты зарегистрированных устройств
 *          ввода/вывода и их протоколы, если они необходимы. \n
 *
 *          Диспетчер позволяет запускать и останавливать все
 *          зарегистрированные устройства одним методом.
 */
class DeviceManager : public Manager {
public:
    ~DeviceManager() { stop(); }

    /**
     * @brief Зарегистрировать устройство ввода/вывода
     * @details Диспетчер автоматически привязывает указанный протокол к
     *          устройству, если он задан
     * @warning Регистрация устройств осуществляется до начала работы
     *          диспетчера
     * @param name Имя устройства
     * @param device Объект устройства ввода/вывода
     * @param protocol Объект протокола информационного взаимодействия, если
     *                 требуется
     */
    void add(std::string name, IODevice_Ptr device,
             Protocol_Ptr protocol = nullptr);
    /**
     * @brief Удалить устройство из списка зарегистрированных
     * @param name Имя устройства
     */
    void remove(const std::string &name) { m_devices.erase(name); }

    /**
     * @brief Получить интерфейс устройства ввода/вывода
     * @param name Имя устройства
     * @return Объект протокола взаимодействия, если он был зарегистрирован,
     *         или объект драйвера устройства в ином случае
     */
    IODevice_Ptr get(const std::string &name);

    /**
     * @brief Начать работу зарегистрированных устройств и протоколов
     * @return true, если информационные обмен всех устройств и протоколов был
     *         успешно запущен
     */
    bool start() override;
    /**
     * @brief Завершить работу зарегистрированных устройств и протоколов
     */
    void stop() override;

private:
    struct Device {
        IODevice_Ptr device;
        Protocol_Ptr protocol;
    };
    std::map<std::string, Device> m_devices;
    bool m_started = false;
};
