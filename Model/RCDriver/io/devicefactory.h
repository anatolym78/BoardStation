/**
 * @file devicefactory.h
 * @brief Объявление фабрики устройств ввода/вывода
 */
#pragma once

#include <cstdint>
#include <string>

#include "iodevice.h"
// #include "radio.h"

/**
 * @class DeviceFactory
 * @brief Фабрика устройств ввода/вывода
 * @details Фабрика предоставляет протой функционал для создания объектов
 *          устройств ввода/вывода. После создания каждого объекта фабрика
 *          перестает владеть созданными объектами.
 */
class DeviceFactory {
public:
    /**
     * @brief Создать драйвер устройства последовательного порта
     * @param port Идентификатор последовательного порта
     * @param baudrate Необходимая скорость передачи данных
     * @return Указатель на объект драйвера устройства последовательного порта
     */
    static IODevice_Ptr createSerial(std::string port, uint32_t baudrate);

    // /**
    //  * @brief Создать драйвер устройства радиомодуля
    //  * @param role Роль устройства
    //  * @return Указатель на объект драйвера устройства радиомодуля
    //  */
    // static IODevice_Ptr createRadio(Radio::Role role);
};
