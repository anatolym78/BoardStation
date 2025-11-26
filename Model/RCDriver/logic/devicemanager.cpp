/**
 * @file devicemanager.cpp
 * @brief Определение класса DeviceManager
 */
#include <QDebug>

#include "devicemanager.h"

static const std::string k_ModuleName = "DeviceManager";

void DeviceManager::add(std::string name, IODevice_Ptr device,
                        Protocol_Ptr protocol) {

    if (protocol) {
        protocol->setDevice(device);
    }

    m_devices.insert({name, {device, protocol}});
}

IODevice_Ptr DeviceManager::get(const std::string &name) {
    auto device = m_devices.find(name);
    if (device == m_devices.end()) {
        return nullptr;
    }

    return device->second.protocol ? device->second.protocol
                                   : device->second.device;
}

bool DeviceManager::start() {
    m_started = true;
    for (const auto &[name, device] : m_devices) {
        m_started &= device.device->start();
        if (device.protocol) {
            m_started &= device.protocol->start();
        }

        if (!m_started) {
            qDebug() << QString::fromStdString(k_ModuleName) << "::start: Failed to start \'" << QString::fromStdString(name) + '\'';

            break;
        }
        else {
            qDebug() << QString::fromStdString(k_ModuleName) << "::start: Device \'" << QString::fromStdString(name) + "\' started";
        }
    }

    return m_started;
}

void DeviceManager::stop() {
    m_started = false;

    for (const auto &[name, device] : m_devices) {
        device.device->stop();
        if (device.protocol) {
            device.protocol->stop();
        }

        qDebug() << QString::fromStdString(k_ModuleName) << "::stop: Device \'" << QString::fromStdString(name) + "\' stopped";
    }
}
