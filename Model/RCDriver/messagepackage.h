#pragma once

#include <string>
#include <memory>
#include <QtConcurrent>

#include "datastructures.h"
#include "io/message.h"



namespace drv {

/**
 * @struct MessagePackage
 * @brief Структура для обработки, десериализации и сериализации данных сообщений
 * @details Содержит вложенные структуры и перечисления, определяющие формат данных,
 *          передаваемых и принимаемых через драйвер. Поддерживает асинхронную сборку
 *          фрагментированных сообщений и преобразование в формат JSON.
 */
struct MessagePackage{
public:

    /**
     * @brief функция записи из vector<uint8_t> в структуру TransmittedData
     * @return состояние
     */
    bool deserializeFromBytes();
    /**
     * @brief Сериализует внутренние данные в формат JSON
     * @return Строка JSON с данными
     */
    std::string serializeToJson();
    /**
     * @brief Проверяет, является ли сообщение полным и обновляет внутреннее состояние
     * @param message Принятое сообщение
     * @return true, если сообщение было полностью собрано и обработано, иначе false
     */
    bool messageFull(ConstMessage message);
    /**
     * @brief Преобразует текущее сообщение в формат JSON
     * @return Строка JSON
     */
    std::string messageToJson();
    /**
     * @brief Получает строку JSON с последними собранными данными
     * @return Строка JSON
     */
    std::string getJsonData();
private:
    TransmittedData data;                   /**< Внутренняя переменная для хранения десериализованных данных */
    std::atomic<int> m_expectedIndex = 0;   /**< Ожидаемый индекс следующего фрагмента сообщения */
    std::array<std::vector<uint8_t>, 3> m_assembledData;   /**< Буфер для сборки фрагментированных сообщений */
    std::mutex m_mutex;                     /**< Мьютекс для синхронизации доступа к данным */
    std::string m_receivedData;             /**< Строка для хранения JSON-представления собранных данных */
};

}
