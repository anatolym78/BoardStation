/**
 * @file driverinterface_impl.h
 * @brief Объявление реализации драйвера для последовательного порта
 */
#pragma once

#include <atomic>
#include <memory>
#include <QString>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include <QString>
#include <cstring>

#include "driverinterface.h"
#include "logic/devicemanager.h"
#include "listeners/messagelistener.h"
#include "io/message.h"
#include "messagepackage.h"
#include "datastructures.h"

namespace drv {

/**
 * @class SerialDriver
 * @brief Реализация драйвера для последовательного порта
 * @details Драйвер для последовательного порта интегрирует работу с
 *          диспетчером устройств и обработчиком сообщений.
 *          Обеспечивает асинхронную работу с устройством через QtConcurrent.
 */
class SerialDriver : public drv::IDriver, public MessageListener, public std::enable_shared_from_this<SerialDriver> {
    Q_OBJECT

public:
    /**
     * @brief Конструктор драйвера последовательного порта
     */
    SerialDriver();

    /**
     * @brief Деструктор драйвера последовательного порта
     */
    ~SerialDriver();

    /**
     * @brief Записать данные в устройство
     * @param data Данные для записи
     * @return true, если сообщение успешно отправлено
     */
    bool write(const std::string &data) override;

    /**
     * @brief Прочитать данные из устройства
     * @return Прочитанные данные
     */
    const std::string &read() override;

    /**
     * @brief Запустить работу драйвера
     * @return true, если драйвер успешно запущен
     */
    bool start() override;

    /**
     * @brief Остановить работу драйвера
     */
    void stop() override;

    /**
     * @brief Установить порт устройства
     * @param m_port Порт устройства
     */
    void addPort(const std::string& m_port) override;

    /**
     * @brief Установить скорость передачи данных
     * @param m_baudrate Скорость передачи данных
     */
    void addBaudrate(int m_baudrate) override;

    /**
     * @brief Обработчик получения сообщения
     * @details Вызывается всякий раз когда отправляется сообщение
     * @param message Принятое сообщение
     */
    void messageReceived(ConstMessage message) override;

private:
    std::string m_receivedData;             /**< Для хранения принятых данных */
    std::string m_deviceId;                 /**< Идентификатор устройства */
    std::string m_port;                     /**< Порт устройства */
    int m_baudrate;                         /**< Скорость передачи данных */
    std::atomic_bool m_running = false;     /**< Флаг работы драйвера */
    std::mutex m_mutex;                     /**< Мьютекс для синхронизации доступа к данным */
    MessagePackage m_data;

    State status;                           /**< Состояние драйвера */
    QFuture<void> m_contextFuture;          /**< Бъект асинхронного выполнения */

    DeviceManager m_deviceManager;          /**< Диспетчер устройств */

    // вспомогательный метод для выполнения работы в отдельном потоке
    void runContextLoop();
    // вспомогательная функция для проверки сообщения при отправке
    bool isValidNeuroStabMode(int value);
};

}
