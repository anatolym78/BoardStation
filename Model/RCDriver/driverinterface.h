/**
 * @file driverinterface.h
 * @brief Объявление интерфейса драйвера
 */
#pragma once

#include <string>
#include <QObject>

namespace drv {

/**
 * @class IDriver
 * @brief Интерфейс драйвера устройства ввода/вывода
 * @details Интерфейс предоставляет основные методы для взаимодействия с
 *          устройствами ввода/вывода, такие как запись и чтение данных.
 *          Используется сигналы для уведомления об изменении состояния и
 *          доступности данных.
 */

class IDriver : public QObject{
    Q_OBJECT

public:
    /**
     * @enum State
     * @brief Состояния драйвера
     */
    enum class State
    {
        k_Connected,   /**< Устройство подключено */
        k_Disconnected /**< Устройство отключено */
    };

    virtual ~IDriver() = default;

    /**
     * @brief Установить порт устройства
     * @param m_port Порт устройства
     */
    virtual void addPort(const std::string& m_port) = 0;
    /**
     * @brief Установить скорость передачи данных
     * @param m_baudrate Скорость передачи данных
     */
    virtual void addBaudrate(int m_baudrate) = 0;

    /**
     * @brief Запустить работу драйвера
     * @return true, если драйвер успешно запущен
     */
    virtual bool start() = 0;

    /**
     * @brief Остановить работу драйвера
     */
    virtual void stop() = 0;

    /**
     * @brief Записать данные в устройство
     * @param data Данные для записи
     * @return true, если сообщение успешно отправлено
     */
    virtual bool write(const std::string &data) = 0;

    /**
     * @brief Прочитать данные из устройства
     * @return Прочитанные данные
     */
    virtual const std::string &read() = 0;

signals:
    /**
     * @brief Сигнал, испускаемый при доступности новых данных
     */
    void dataAvailable();

    /**
     * @brief Сигнал, испускаемый при изменении состояния драйвера
     * @param state Новое состояние драйвера
     */
    void stateChanged(State state);

protected:
    /**
     * @brief Испускает сигнал dataAvailable
     */
    void emitDataAvailable() { emit dataAvailable(); }

    /**
     * @brief Испускает сигнал stateChanged
     * @param state Новое состояние драйвера
     */
    void emitStateChanged(State state) { emit stateChanged(state); }
};

}
