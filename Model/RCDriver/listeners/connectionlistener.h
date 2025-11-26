/**
 * @file connectionlistener.h
 * @brief Объявление интерфейса наблюдателя за состоянием соединения
 */
#pragma once

#include "io/connection.h"
#include "datalistener.h"

/**
 * @class ConnectionListener
 * @brief Интерфейс наблюдателя за состоянием соединения
 * @details Интерфейс обновляется каждый раз, когда происходит изменение
 *          состояния некоторого соединения
 */
class ConnectionListener : public DataListener<ConnectionStatus> {
public:
    virtual ~ConnectionListener() = default;

    /**
     * @brief Обработать новый статус соединения
     * @param status Новый статус соединения
     */
    virtual void dataUpdated(const ConnectionStatus &status) override = 0;
};
