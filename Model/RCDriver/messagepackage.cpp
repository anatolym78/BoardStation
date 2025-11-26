#include <cstring>

#include "messagepackage.h"


std::string drv::MessagePackage::messageToJson(){
    return serializeToJson();
}

// собираем из нескольких сообщений общую структуру
bool drv::MessagePackage::messageFull(ConstMessage message){
    //проверяем нет пустая ли структура
    //актуальный ли указатель || проверяем не пустой ли сам вектор
    if (!message || message->empty()){
        return false;
    }
    //ставим mutex чтобы не было гонки данных
    std::lock_guard<std::mutex> lock(m_mutex);

    //первый байт в десятичной системе равен
    int index = static_cast<int>(message->at(0));

    if (index < 0 || index >= 3){
        return false;
    }

    m_assembledData.at(index).assign(message->begin() + 1, message->end());

    if (!deserializeFromBytes()){
        return false;
    }
    m_receivedData = messageToJson();
    return true;
}

//функция записи из vector<uint8_t> в структуру TransmittedData
bool drv::MessagePackage::deserializeFromBytes(){
    // Собираем все байты в один вектор
    std::vector<uint8_t> flat_data;
    for (const auto& vec : m_assembledData) {
        flat_data.insert(flat_data.end(), vec.begin(), vec.end());
    }
    if (flat_data.size() == sizeof(TransmittedData)) {
        std::memcpy(&data, flat_data.data(), sizeof(TransmittedData));
        return true;
    }
    qDebug() << "Недостаточно байтов для десериализации!";
    return false;
}

std::string drv::MessagePackage::serializeToJson(){
    QJsonArray j;

    // Создаем локальные копии, чтобы избежать передачи ссылки на упакованные поля
    int32_t sv = data.softVersion;
    uint8_t dr_stab = data.dataRates.stabilization;
    uint8_t dr_telem = data.dataRates.telemetry;
    uint32_t ts_ms = data.telemetry.timeStamp_ms;
    uint32_t fm = data.telemetry.flightMode;
    int16_t gs = data.telemetry.groundSpeed;
    int32_t alt = data.telemetry.altitude;
    uint16_t bv = data.telemetry.batteryVoltage;
    int16_t thr_adj[k_EnginesCount];
    std::copy(data.stabData.throttleAdjustmentValues, data.stabData.throttleAdjustmentValues + k_EnginesCount, thr_adj);

    j.append(QJsonObject{{"label", "softVersion"}, {"value", static_cast<int>(sv)}});
    j.append(QJsonObject{{"label", "dataRates.stabilization"}, {"value", static_cast<int>(dr_stab)}});
    j.append(QJsonObject{{"label", "dataRates.telemetry"}, {"value", static_cast<int>(dr_telem)}});
    j.append(QJsonObject{{"label", "neuroStabMode"}, {"value", static_cast<int>(data.neuroStabMode)}});
    j.append(QJsonObject{{"label", "telemetry.timeStamp_ms"}, {"value", static_cast<int>(ts_ms)}});
    j.append(QJsonObject{{"label", "telemetry.flightMode"}, {"value", static_cast<int>(fm)}});
    j.append(QJsonObject{{"label", "telemetry.stabilizationMode"}, {"value", static_cast<int>(data.telemetry.stabilizationMode)}});

    // Используем QJsonArray для массивов
    QJsonArray accArray;
    accArray.append(data.telemetry.acceleration[0]);
    accArray.append(data.telemetry.acceleration[1]);
    accArray.append(data.telemetry.acceleration[2]);
    j.append(QJsonObject{{"label", "telemetry.acceleration"}, {"value", accArray}});

    j.append(QJsonObject{{"label", "telemetry.groundSpeed"}, {"value", static_cast<int>(gs)}});

    QJsonArray absPosArray;
    absPosArray.append(data.telemetry.absolutePosition[0]);
    absPosArray.append(data.telemetry.absolutePosition[1]);
    absPosArray.append(data.telemetry.absolutePosition[2]);
    j.append(QJsonObject{{"label", "telemetry.absolutePosition"}, {"value", absPosArray}});

    j.append(QJsonObject{{"label", "telemetry.altitude"}, {"value", static_cast<int>(alt)}});

    QJsonArray gpsLocArray;
    gpsLocArray.append(data.telemetry.gpsLocation[0]);
    gpsLocArray.append(data.telemetry.gpsLocation[1]);
    gpsLocArray.append(data.telemetry.gpsLocation[2]);
    j.append(QJsonObject{{"label", "telemetry.gpsLocation"}, {"value", gpsLocArray}});

    j.append(QJsonObject{{"label", "telemetry.batteryVoltage"}, {"value", static_cast<int>(bv)}});
    j.append(QJsonObject{{"label", "stabData.isArmed"}, {"value", data.stabData.isArmed}});

    QJsonArray gyroRatesArray;
    gyroRatesArray.append(data.stabData.gyroAngleRates[0]);
    gyroRatesArray.append(data.stabData.gyroAngleRates[1]);
    gyroRatesArray.append(data.stabData.gyroAngleRates[2]);
    j.append(QJsonObject{{"label", "stabData.gyroAngleRates"}, {"value", gyroRatesArray}});

    QJsonArray desiredRatesArray;
    desiredRatesArray.append(data.stabData.desiredAngleRates[0]);
    desiredRatesArray.append(data.stabData.desiredAngleRates[1]);
    desiredRatesArray.append(data.stabData.desiredAngleRates[2]);
    j.append(QJsonObject{{"label", "stabData.desiredAngleRates"}, {"value", desiredRatesArray}});

    QJsonArray thrAdjArray;
    thrAdjArray.append(thr_adj[0]);
    thrAdjArray.append(thr_adj[1]);
    thrAdjArray.append(thr_adj[2]);
    thrAdjArray.append(thr_adj[3]);
    j.append(QJsonObject{{"label", "stabData.throttleAdjustmentValues"}, {"value", thrAdjArray}});

    //возвращаем json в виде string
    return QJsonDocument(j).toJson(QJsonDocument::Indented).toStdString();
}

std::string drv::MessagePackage::getJsonData(){
    return m_receivedData;
}
