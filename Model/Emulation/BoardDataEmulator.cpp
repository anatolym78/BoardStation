#include "BoardDataEmulator.h"
#include "BoardDataGenerators/AltitudeGenerator.h"
#include "BoardDataGenerators/LongitudeGenerator.h"
#include "BoardDataGenerators/LatitudeGenerator.h"
#include "BoardDataGenerators/SpeedGenerator.h"
#include "BoardDataGenerators/ThrottleAdjustmentValuesGenerator.h"
#include "BoardDataGenerators/BoardDataJsonGenerator.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace drv 
{

    BoardDataEmulator::BoardDataEmulator(QObject *)
        : m_stateTimer(new QTimer(this))
        , m_dataTimer(new QTimer(this))
        , m_jsonWriter(new BoardDataJsonGenerator(this))
        , m_time(0.0)
        , m_currentState(State::k_Disconnected)
        , m_isRunning(false)
        , m_isListening(true) // По умолчанию слушаем
    {
        // Настройка таймера состояния (каждые 30 секунд)
        m_stateTimer->setInterval(30000);
        connect(m_stateTimer, &QTimer::timeout, this, &BoardDataEmulator::onStateTimerTimeout);
    
        // Настройка таймера данных (каждую секунду)
        m_dataTimer->setInterval(1000);
        connect(m_dataTimer, &QTimer::timeout, this, &BoardDataEmulator::onDataTimerTimeout);
    
        // Настраиваем генераторы параметров
        setupGenerators();
    
        // Автоматически запускаем эмулятор
        start();
    }

    BoardDataEmulator::~BoardDataEmulator()
    {
        stop();
    
        // Очищаем генераторы
        qDeleteAll(m_generators);
        m_generators.clear();
    }

    void BoardDataEmulator::write(const std::string &data)
    {
        // Добавляем входящее сообщение в очередь
        m_messageQueue.enqueue(data);
    
        // Сохраняем отправленные параметры в файл
        QString jsonString = QString::fromStdString(data);
        saveSentParametersToFile(jsonString);
    
        // Эмитируем сигнал с отправленными данными
        emitDataSent(jsonString);
    }

    const std::string &BoardDataEmulator::read()
    {
        static const std::string emptyString;
    
        if (m_messageQueue.isEmpty()) {
            return emptyString;
        }
    
        // Извлекаем последнее пришедшее сообщение
        return m_messageQueue.last();
    }

    void BoardDataEmulator::flushData()
    {
        m_messageQueue.clear();
    }

    void BoardDataEmulator::start()
    {
        if (m_isRunning) {
            return;
        }
    
        m_isRunning = true;
        m_time = 0.0;
    
        // Запускаем таймеры
        //m_stateTimer->start();
        m_dataTimer->start();
    
        // Устанавливаем начальное состояние
        m_currentState = State::k_Connected;
        emitStateChanged(m_currentState);
    
        //qDebug() << "BoardDataEmulator started";
    }

    void BoardDataEmulator::stop()
    {
        if (!m_isRunning) {
            return;
        }
    
        m_isRunning = false;
    
        // Останавливаем таймеры
        //m_stateTimer->stop();
        m_dataTimer->stop();
    
        // Очищаем очередь
        flushData();
    
        //qDebug() << "BoardDataEmulator stopped";
    }

    void BoardDataEmulator::onStateTimerTimeout()
    {
        if (!m_isRunning) {
            return;
        }
    
        // Переключаем состояние между connected и disconnected
        m_currentState = (m_currentState == State::k_Connected) ? 
                            State::k_Disconnected : State::k_Connected;
    
        emitStateChanged(m_currentState);
    
        //qDebug() << "BoardDataEmulator state changed to:" << (m_currentState == State::k_Connected ? "Connected" : "Disconnected");
    }

    void BoardDataEmulator::onDataTimerTimeout()
    {
        if (!m_isRunning || m_currentState != State::k_Connected || !m_isListening) {
            return;
        }
    
        // Генерируем параметры
        generateParameters();
    
        // Увеличиваем время
        m_time += 1.0;
    }

    void BoardDataEmulator::setupGenerators()
    {
        ThrottleAdjustmentValuesGenerator* throttleGen = new ThrottleAdjustmentValuesGenerator(this);
        throttleGen->setupThrottleParameter(0, 10, 5, 0);
        throttleGen->setupThrottleParameter(1, 15, 7, M_PI / 3);
        throttleGen->setupThrottleParameter(2, 20, 10, M_PI);
        throttleGen->setupThrottleParameter(3, 12, 4, 3 * M_PI / 2);
        m_generators.append(throttleGen);

        //qDebug() << "BoardDataEmulator: Настроены генераторы параметров:" << m_generators.size();
    }

    void BoardDataEmulator::generateParameters()
    {
        QList<BoardParameterSingle*> parameters;
    
        // Генерируем параметры от всех генераторов
        for (ParameterGenerator *generator : m_generators)
        {
            BoardParameterSingle *param = generator->generate(m_time);
            if (param) 
            {
                parameters.append(param);
            }
        }
    
        if (parameters.isEmpty())
        {
            //qDebug() << "BoardDataEmulator: Не удалось сгенерировать параметры";
            qDeleteAll(parameters);
            return;
        }
    
        // Создаем JSON строку из параметров
        QString jsonString = m_jsonWriter->createJsonString(parameters);
    
        if (!jsonString.isEmpty()) 
        {
            // Добавляем JSON строку в очередь сообщений
            std::string jsonData = jsonString.toStdString();
            m_messageQueue.enqueue(jsonData);
        
            // Генерируем сигнал о доступности данных
            emitDataAvailable();
        
            //qDebug() << "BoardDataEmulator: Сгенерированы параметры, JSON:" << jsonString;
        }

        qDeleteAll(parameters);
    }

    void BoardDataEmulator::saveSentParametersToFile(const QString &jsonString)
    {
        QString filename = QApplication::applicationDirPath() + "/TestSendParameters.json";
    
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "BoardDataEmulator: Cannot open file for writing:" << filename;
            return;
        }
    
        // Парсим JSON и переформатируем его для читаемости
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "BoardDataEmulator: JSON parse error:" << error.errorString();
            // Если не удалось распарсить, сохраняем как есть
            QTextStream stream(&file);
            stream << jsonString;
        } else {
            // Сохраняем в читаемом формате
            QByteArray formattedJson = doc.toJson(QJsonDocument::Indented);
            file.write(formattedJson);
        }
    
        file.close();
    
        //qDebug() << "BoardDataEmulator: Sent parameters saved to" << filename;
        //qDebug() << "BoardDataEmulator: JSON content:" << jsonString;
    }

    // Реализации функций управления прослушиванием

    void BoardDataEmulator::startListening()
    {
        m_isListening = true;
        qDebug() << "BoardDataEmulator: Started listening";
    }

    void BoardDataEmulator::stopListening()
    {
        m_isListening = false;
        qDebug() << "BoardDataEmulator: Stopped listening";
    }

    bool BoardDataEmulator::isListening() const
    {
        return m_isListening;
    }

} // namespace drv

