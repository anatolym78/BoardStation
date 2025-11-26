#pragma once

#include <atomic>
#include <cstring>


//структуры для составления принятого пакета
enum class NeuroStabMode : uint8_t {
    k_Pid,                      /**< Режим PID-стабилизации */
    k_DeepLearning,             /**< Режим стабилизации с использованием глубокого обучения */
    k_ReinforcementLearning,    /**< Режим стабилизации с использованием обучения с подкреплением */
    k_Ann,                      /**< Режим стабилизации с использованием нейросети */
    k_Manual                    /**< Ручной режим */
};

/**
     * @enum StabilizationMode
     * @brief Битовые флаги режимов стабилизации
     */
enum class StabilizationMode : uint8_t {
    k_Pid       = (1 << 0),     /**< PID-стабилизация */
    k_NeuroStab = (1 << 1)      /**< Нейросетевая стабилизация */
};

static const int k_AxisCount = 3;
static const int k_EnginesCount = 4;

/**
     * @struct StabilizationData
     * @brief Данные, связанные с процессом стабилизации
     * @details Структура упакована для компактной передачи по сети/интерфейсу.
     */
struct __attribute__((packed)) StabilizationData {
    bool isArmed = false;
    float gyroAngleRates[k_AxisCount];
    float desiredAngleRates[k_AxisCount];
    int16_t throttleAdjustmentValues[k_EnginesCount];
};

/**
     * @struct TelemetryData
     * @brief Данные телеметрии
     * @details Содержит информацию о состоянии устройства. Структура упакована.
     */
struct __attribute__((packed)) TelemetryData {
    uint32_t timeStamp_ms = 0;
    uint32_t flightMode = 0;
    StabilizationMode stabilizationMode = StabilizationMode::k_Pid;
    float acceleration[k_AxisCount];
    int16_t groundSpeed = 0;
    int16_t absolutePosition[k_AxisCount];
    int32_t altitude = 0;
    int32_t gpsLocation[k_AxisCount];
    uint16_t batteryVoltage = 0;
};

/**
     * @struct TransmittedData
     * @brief Полная структура данных, передаваемых/принимаемых по интерфейсу
     * @details Содержит версию ПО, частоты данных, режим нейростабилизации, телеметрию и данные стабилизации.
     *          Структура упакована.
     */
struct __attribute__((packed)) TransmittedData {
    int32_t softVersion = 1;
    struct __attribute__((packed)) {
        uint8_t stabilization = 0;
        uint8_t telemetry = 0;
    } dataRates;
    NeuroStabMode neuroStabMode = NeuroStabMode::k_Pid;
    TelemetryData telemetry;
    StabilizationData stabData;
};


struct __attribute__((packed)) ControlData {
    NeuroStabMode neuroStabMode = NeuroStabMode::k_Pid;
    int16_t throttleAdjustmentValues[k_EnginesCount];

    /* Функция создания структуры и инициализации массива */
    static ControlData create() {
        ControlData data;
        std::memset(data.throttleAdjustmentValues, 0,
                    sizeof(throttleAdjustmentValues));

        return data;
    }
};
