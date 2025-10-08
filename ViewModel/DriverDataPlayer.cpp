#include "DriverDataPlayer.h"
#include <QDebug>
#include <QDateTime>

DriverDataPlayer::DriverDataPlayer(QObject *parent)
    : DataPlayer(parent)
    , m_isInitialized(false)
{
    // Устанавливаем имя сессии для режима реального времени
    m_currentSessionName = "Live Data";
    emit currentSessionNameChanged();

    // В режиме реального времени не запускаем таймер автоматически
    // Позиция будет обновляться при получении новых параметров
}

DriverDataPlayer::~DriverDataPlayer()
{
    stop();
}

void DriverDataPlayer::play()
{
    // В режиме реального времени просто устанавливаем флаг воспроизведения
    // Позиция обновляется при получении новых параметров
    if (!m_isPlaying)
    {
        m_isPlaying = true;
        m_shouldStop = 0;
        startPlayback();
        emit isPlayingChanged();
    }
}

void DriverDataPlayer::stop()
{
    DataPlayer::stop();
    
    // Сбрасываем состояние инициализации
    m_isInitialized = false;
}

void DriverDataPlayer::pause()
{
    // В режиме реального времени просто останавливаем воспроизведение
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_shouldStop = 1;
        stopPlayback();
        emit isPlayingChanged();
    }
}

void DriverDataPlayer::setPosition(QDateTime position)
{
    // В режиме реального времени ограничиваем позицию текущим временем
    if (position < m_sessionStartTime)
    {
        position = m_sessionStartTime;
    }
    else if (position > m_sessionEndTime)
    {
        position = m_sessionEndTime;
    }
    
    m_currentPosition = position;
    emit currentPositionChanged();
    emit elapsedTimeChanged();
}

void DriverDataPlayer::setStorage(BoardParameterHistoryStorage* storage)
{
    // Вызываем базовую реализацию
    DataPlayer::setStorage(storage);
    
    // Подключаемся к сигналу новых параметров
    if (storage)
    {
        connect(storage, &BoardParameterHistoryStorage::newParameterAdded,
                this, &DriverDataPlayer::onNewParameterAdded);
    }
}

void DriverDataPlayer::onNewParameterAdded(BoardParameterSingle* parameter)
{
    if (!parameter)
    {
        return;
    }
    
    // Инициализируем временной диапазон при получении первого параметра
    if (!m_isInitialized)
    {
        initializeTimeRange();

        play();
    }

    QDateTime paramTime = parameter->timestamp();

    if (m_sessionStartTime.isNull())
    {
        m_sessionStartTime = QDateTime(paramTime);

        extendTimeRange();
    }
    
    // Проверяем, нужно ли расширить временной диапазон
    if (paramTime >= m_sessionEndTime)
    {
        extendTimeRange();
    }
    
    // Проверяем и проигрываем параметры, если плеер активен
    checkAndPlayParameters();
}

void DriverDataPlayer::updatePlaybackPosition()
{
    // В режиме реального времени обновляем позицию на текущее время
    QDateTime currentTime = QDateTime::currentDateTime();
    
    // Ограничиваем позицию в пределах сессии
    if (currentTime < m_sessionStartTime)
    {
        currentTime = m_sessionStartTime;
    }
    else if (currentTime > m_sessionEndTime)
    {
        currentTime = m_sessionEndTime;
    }
    
    m_currentPosition = currentTime;
    
    // Проверяем и проигрываем параметры
    checkAndPlayParameters();
    
    emit currentPositionChanged();
    emit elapsedTimeChanged();
}

void DriverDataPlayer::checkAndPlayParameters()
{
    if (!m_storage || !m_isPlaying)
    {
        return;
    }
    
    // Получаем все параметры из хранилища
    QList<BoardParameterSingle*> params = m_storage->getSessionParameters();
    
    // Проигрываем параметры, время которых меньше или равно текущей позиции
    for (BoardParameterSingle* param : params)
    {
        if (param && param->timestamp() <= m_currentPosition)
        {
            emit parameterPlayed(param);
        }
    }
}

void DriverDataPlayer::extendTimeRange()
{
    // Расширяем конечное время на 10 минут
    m_sessionEndTime = m_sessionEndTime.addSecs(60*TIME_RANGE_MINUTES);
    
    emit sessionEndTimeChanged();
    emit sessionDurationChanged();
    
    qDebug() << "DriverDataPlayer: Extended time range to" << m_sessionEndTime.toString();
}

void DriverDataPlayer::initializeTimeRange()
{
    if (!m_storage)
    {
        return;
    }
    
    QList<BoardParameterSingle*> params = m_storage->getSessionParameters();
    if (params.isEmpty())
    {
        return;
    }
    
    // Устанавливаем время начала на основе первого параметра
    BoardParameterSingle* firstParam = params.first();
    if (firstParam)
    {
        m_sessionStartTime = firstParam->timestamp();
        m_sessionEndTime = m_sessionStartTime.addSecs(60*TIME_RANGE_MINUTES);
        m_currentPosition = m_sessionStartTime;
        
        m_isInitialized = true;
        
        emit sessionStartTimeChanged();
        emit sessionEndTimeChanged();
        emit sessionDurationChanged();
        emit currentPositionChanged();
        emit elapsedTimeChanged();
        
        qDebug() << "DriverDataPlayer: Initialized time range from" 
                 << m_sessionStartTime.toString() << "to" << m_sessionEndTime.toString();
    }
}
