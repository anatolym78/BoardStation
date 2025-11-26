/**
 * @file connection.h
 * @brief Объявление перечислений, описывающих состояния соединений
 */
#pragma once

/**
 * @enum ConnectionStatus
 * @brief Состояние соединения
 */
enum class ConnectionStatus {
    k_Connected,    ///< Соединение установлено
    k_Disconnected, ///< Соединение отсутствует

    k_Undefined     ///< Состояние не определено (устройство отсутствует и т.п.)
};
