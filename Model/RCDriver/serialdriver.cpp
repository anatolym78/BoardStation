/**
 * @file driverinterface_impl.cpp
 * @brief Определение реализации драйвера для последовательного порта и записи данных в json
 */
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "serialdriver.h"
#include "logic/devicemanager.h"
#include "io/devicefactory.h"
#include "io/context.h"
#include "driver/protocolfactory.h"
#include "listeners/listener.h"



drv::SerialDriver::SerialDriver() : m_deviceId("PICO_serial"){
}

drv::SerialDriver::~SerialDriver() {
    stop();
}

void drv::SerialDriver::runContextLoop() {
    while (m_running) {
        if (m_deviceManager.start()) {
            emitStateChanged(State::k_Connected);

            try {
                Context::get().run();

                m_deviceManager.stop();
                Context::get().stop();
            } catch (...) {
                emitStateChanged(State::k_Disconnected);

                m_deviceManager.stop();
                Context::get().stop();
            }
        } else {
            m_deviceManager.stop();
            emitStateChanged(State::k_Disconnected);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

bool drv::SerialDriver::start(){
    if (m_running) {
        return false;
    }
    m_running = true;

    m_deviceManager.add(m_deviceId,
                        DeviceFactory::createSerial(m_port, m_baudrate),
                        ProtocolFactory::createSerialProtocol());
    m_deviceManager.get(this->m_deviceId)->attach(shared_from_this());

    m_contextFuture.cancel();
    // Запускаем m_contextFuture из метода start(), передавая ему приватный метод runContextLoop
    m_contextFuture = QtConcurrent::run(this, &SerialDriver::runContextLoop);

    return true;
}

void drv::SerialDriver::stop() {
    emitStateChanged(State::k_Disconnected);

    m_running = false;
    m_deviceManager.stop();

    if (m_contextFuture.isRunning()){
        m_contextFuture.waitForFinished();
    }

    Context::get().stop();
    m_deviceManager.remove(m_deviceId);
}

void drv::SerialDriver::addPort(const std::string& m_port){
    this->m_port = m_port;
}

void drv::SerialDriver::addBaudrate(int m_baudrate){
    this->m_baudrate = m_baudrate;
}

void drv::SerialDriver::messageReceived(ConstMessage message){

    if (m_data.messageFull(message)){
        //ставим mutex чтобы не было гонки данных
        std::lock_guard<std::mutex> lock(m_mutex);
        m_receivedData = m_data.getJsonData();
        qDebug() << QString::fromStdString(m_receivedData);
        emitDataAvailable();
    }
}

const std::string &drv::SerialDriver::read() {
    //ставим mutex чтобы не было гонки данных
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_receivedData;
}

// Вспомогательная функция для проверки сообщения при отправке
bool drv::SerialDriver::isValidNeuroStabMode(int value) {
    switch (static_cast<NeuroStabMode>(value)) {
    case NeuroStabMode::k_Pid:
    case NeuroStabMode::k_DeepLearning:
    case NeuroStabMode::k_ReinforcementLearning:
    case NeuroStabMode::k_Ann:
    case NeuroStabMode::k_Manual:
        return true;
    }
    return false;
}

bool drv::SerialDriver::write(const std::string &data) {
    // Преобразуем строку JSON в QByteArray для парсинга Qt
    QByteArray jsonData = QByteArray::fromStdString(data);

    QJsonParseError parseError;
    // Разбираем строку JSON в документ
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    // Проверяем, не было ли ошибки парсинга
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse Error ";
        return false;
    }

    // Проверяем, является ли корневой элемент документа массивом
    if (!jsonDoc.isArray()) {
        qDebug() << "JSON is not an array.";
        return false;
    }

    // Получаем массив объектов
    QJsonArray jsonArray = jsonDoc.array();
    // Создаем структуру ControlData, инициализированную значениями по умолчанию
    ControlData controlData = ControlData::create();

    for (const QJsonValue & value : jsonArray) {
        // Пропускаем, если элемент не является объектом
        if (!value.isObject()) {
            continue;
        }

        // Преобразуем элемент в объект JSON
        QJsonObject obj = value.toObject();
        // Извлекаем метку (label) и значение (value)
        QString label = obj["label"].toString();
        QJsonValue val = obj["value"];

        // Обрабатываем NeuroStabMode
        if (label == "neuroStabMode") {
            // Проверяем, является ли значение числом
            if (val.isDouble()) {
                int modeValue = val.toInt();
                // Проверяем, допустимо ли числовое значение для enum
                if (isValidNeuroStabMode(modeValue)) {
                    // Устанавливаем значение в структуру
                    controlData.neuroStabMode = static_cast<NeuroStabMode>(modeValue);
                } else {
                    qDebug() << "Warning: Invalid NeuroStabMode value: " << modeValue;
                    return false;
                }
            } else {
                qDebug() << "Error: neuroStabMode value is not a number.";
                return false;
            }
        }

        // Обрабатываем throttleAdjustmentValues
        if (label == "throttleAdjustmentValues") {
            // Проверяем, является ли значение массивом
            if (val.isArray()) {
                QJsonArray thrArray = val.toArray();
                // Временный вектор для хранения значений из JSON
                std::vector<int16_t> tempValues;
                tempValues.reserve(thrArray.size()); // Оптимизация: предварительно выделяем память

                // Проходим по каждому элементу массива значений
                for (const QJsonValue & thrVal : thrArray) {
                    // Проверяем, является ли элемент числом
                    if (thrVal.isDouble()) {
                        // Преобразуем и добавляем в вектор
                        tempValues.push_back(static_cast<int16_t>(thrVal.toInt()));
                    } else {
                        qDebug() << "Warning: throttleAdjustmentValues contains non-number.";
                        return false;
                    }
                }

                // Определяем количество элементов для копирования, ограниченное размером массива в структуре
                size_t count = std::min(tempValues.size(), static_cast<size_t>(k_EnginesCount));
                // Копируем значения из временного вектора в массив структуры
                std::copy_n(tempValues.begin(), count, controlData.throttleAdjustmentValues);

            } else {
                qDebug() << "Error: throttleAdjustmentValues value is not an array.";
                return false;
            }
        }
    }

    // Выводим значение neuroStabMode для отладки
    qDebug() << int(controlData.neuroStabMode);

    // Выводим значения throttleAdjustmentValues для отладки
    for (int i = 0; i < k_EnginesCount; i++){
        qDebug() << controlData.throttleAdjustmentValues[i];
    }

    // Создаем вектор байтов для бинарной передачи
    MessageData binaryData;
    // Изменяем размер вектора, чтобы он мог вместить всю структуру
    binaryData.resize(sizeof(ControlData));

    // Копируем байты из структуры в вектор
    std::memcpy(binaryData.data(), &controlData, sizeof(ControlData));
    // Создаем ConstMessage (shared_ptr<const MessageData>) из вектора, передавая его владение
    ConstMessage messageToSend = std::make_shared<const MessageData>(std::move(binaryData));

    // Отправляем бинарное сообщение через m_deviceManager
    m_deviceManager.get(this->m_deviceId)->write(messageToSend);

    return true;
}
