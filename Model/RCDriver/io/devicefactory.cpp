/**
 * @file devicefactory.cpp
 * @brief Определение фабрики устройств ввода/вывода
 */
#include "devicefactory.h"

// #include "radiodevice.h"
#include "serialport.h"

IODevice_Ptr DeviceFactory::createSerial(std::string port, uint32_t baudrate) {
    SerialPort *device = new SerialPort;
    device->setPort(port);
    device->setBaudrate(baudrate);

    return IODevice_Ptr(device);
}

// IODevice_Ptr DeviceFactory::createRadio(Radio::Role role) {
//     RadioDevice *device = new RadioDevice;
//     device->setRole(role);

//     return IODevice_Ptr(device);
// }
