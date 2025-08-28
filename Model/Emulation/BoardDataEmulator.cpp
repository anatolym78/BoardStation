#include "BoardDataEmulator.h"
#include "AltitudeGenerator.h"
#include "LongitudeGenerator.h"
#include "LatitudeGenerator.h"
#include "SpeedGenerator.h"
#include <QDebug>

namespace drv {

BoardDataEmulator::BoardDataEmulator(QObject *)
    : m_stateTimer(new QTimer(this))
    , m_dataTimer(new QTimer(this))
    , m_jsonWriter(new JsonWriter(this))
    , m_time(0.0)
    , m_currentState(State::k_Disconnected)
    , m_isRunning(false)
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
    
    qDebug() << "BoardDataEmulator started";
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
    
    qDebug() << "BoardDataEmulator stopped";
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
    
    qDebug() << "BoardDataEmulator state changed to:" << (m_currentState == State::k_Connected ? "Connected" : "Disconnected");
}

void BoardDataEmulator::onDataTimerTimeout()
{
    if (!m_isRunning || m_currentState != State::k_Connected) {
        return;
    }
    
    // Генерируем параметры
    generateParameters();
    
    // Увеличиваем время
    m_time += 1.0;
}

void BoardDataEmulator::setupGenerators()
{
    // Создаем и настраиваем генератор высоты
    AltitudeGenerator *altitudeGen = new AltitudeGenerator(this);
    altitudeGen->setBaseAltitude(500.0);
    altitudeGen->setAmplitude(2.0);
    altitudeGen->setPeriod(5.0);
    
    // Создаем и настраиваем генератор долготы
    LongitudeGenerator *longitudeGen = new LongitudeGenerator(this);
    longitudeGen->setStartLongitude(37.6173);  // Москва
    longitudeGen->setSpeed(0.001);             // Медленное движение
    
    // Создаем и настраиваем генератор широты
    LatitudeGenerator *latitudeGen = new LatitudeGenerator(this);
    latitudeGen->setStartLatitude(55.7558);    // Москва
    latitudeGen->setSpeed(0.0005);             // Медленное движение
    
    // Создаем и настраиваем генератор скорости
    SpeedGenerator *speedGen = new SpeedGenerator(this);
    speedGen->setBaseSpeed(50.0);              // Базовая скорость 50 м/с
    speedGen->setAmplitude(5.0);               // Колебания ±5 м/с
    speedGen->setPeriod(3.0);                  // Период 3 секунды
    
    // Добавляем все генераторы в список
    m_generators.append(altitudeGen);
    m_generators.append(longitudeGen);
    m_generators.append(latitudeGen);
    m_generators.append(speedGen);
    
    qDebug() << "BoardDataEmulator: Настроены генераторы параметров:" << m_generators.size();
}

void BoardDataEmulator::generateParameters()
{
    QList<Parameter> parameters;
    
    // Генерируем параметры от всех генераторов
    for (ParameterGenerator *generator : m_generators) {
        Parameter param = generator->generate(m_time);
        parameters.append(param);
    }
    
    if (parameters.isEmpty()) {
        qDebug() << "BoardDataEmulator: Не удалось сгенерировать параметры";
        return;
    }
    
    // Создаем JSON строку из параметров
    QString jsonString = m_jsonWriter->createJsonString(parameters);
    
    if (!jsonString.isEmpty()) {
        // Добавляем JSON строку в очередь сообщений
        std::string jsonData = jsonString.toStdString();
        m_messageQueue.enqueue(jsonData);
        
        // Генерируем сигнал о доступности данных
        emitDataAvailable();
        
        qDebug() << "BoardDataEmulator: Сгенерированы параметры, JSON:" << jsonString;
    }
}

} // namespace drv
