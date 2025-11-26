/**
 * @file serial.h
 * @brief Объявление интерфейса последовательного порта
 */
#pragma once

#include <cstdint>
#include <string>

#include "iodevice.h"

/**
 * @class Serial
 * @brief Интерфейс последовательного порта
 * @details Интерфейс предоставляет доступ к базовому функционалу
 *          последовательного порта. \n
 *
 *          Реализация порта должна поддерживать следующие типы наблюдателей:
 *          ConnectionListener, MessageListener.
 */
class Serial : public IODevice {
public:
    virtual ~Serial() = default;

    /**
     * @brief Задать идентификатор последовательного порта
     * @param port Идентификатор последовательного порта
     */
    virtual void setPort(std::string port) = 0;
    /**
     * @brief Задать скорость передачи данных
     * @param baudrate Необходимая скорость передачи данных
     */
    virtual void setBaudrate(uint32_t baudrate) = 0;

    /**
     * @brief Начать работу последовательного порта
     * @details Метод открывает последовательный порт и осуществляет
     *          регистрацию асинхронных операций в контексте.
     *          Для начала выполнения этих операций необходимо самостоятельно
     *          запустить контекст, если он не был запущен до этого.
     * @return false, если не удалось инициализировать порт
     */
    virtual bool start() override = 0;
    /**
     * @brief Завершить работу последовательного порта
     */
    virtual void stop() override = 0;

    /**
     * @brief Уточнить максимальный размер массива передаваемых данных одним
     *        сообщением, поддерживаемый портом
     * @return Максимальный размер массива данных
     */
    virtual std::size_t dataLimit() const noexcept override = 0;

    /**
     * @brief Получить информацию о состоянии соединения порта
     * @return Текущее состояние порта
     */
    virtual ConnectionStatus status() const noexcept override = 0;

    /**
     * @brief Отправить сообщение через порт
     * @param outgoing Сообщение, которое необходимо отправить
     */
    virtual void write(ConstMessage outgoing) override = 0;

    /**
     * @brief Привязать наблюдателя
     * @details Реализация интерфейса порта должна поддерживать следующие типы
     *          наблюдателей: ConnectionListener, MessageListener
     * @param listener Новый наблюдатель
     */
    virtual void attach(Listener_Ptr listener) override = 0;
    /**
     * @brief Отвязать наблюдателя
     * @param listener Наблюдатель, которого необходимо отвязать
     */
    virtual void detach(Listener_Ptr listener) override = 0;
};
