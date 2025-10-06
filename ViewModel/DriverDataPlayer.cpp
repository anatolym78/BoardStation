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
}

DriverDataPlayer::~DriverDataPlayer()
{
    stop();
}

void DriverDataPlayer::play()
{
    // В режиме реального времени просто запускаем таймер
    DataPlayer::play();
}

void DriverDataPlayer::stop()
{
    DataPlayer::stop();
    
    // Сбрасываем состояние инициализации
    m_isInitialized = false;
}

void DriverDataPlayer::pause()
{
    DataPlayer::pause();
}

void DriverDataPlayer::setPosition(double position)
{
    // В режиме реального времени ограничиваем позицию текущим временем
    position = qBound(0.0, position, m_maxPosition);
    
    m_currentPosition = position;
    emit currentPositionChanged();
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
    }

	QDateTime paramTime = parameter->timestamp();

    if (m_sessionStartTime.isNull())
	{
        m_sessionStartTime = QDateTime(paramTime);

        extendTimeRange();
    }


    
    // Обновляем текущую позицию на основе времени параметра
    qint64 positionMs = m_sessionStartTime.msecsTo(paramTime);
    m_currentPosition = positionMs / 1000.0;
    
    // Проверяем, нужно ли расширить временной диапазон
    if (paramTime >= m_sessionEndTime)
    {
        extendTimeRange();
    }
    
    // Эмитируем сигнал о проигрывании параметра
    emit parameterPlayed(parameter);
    
    emit currentPositionChanged();
}

void DriverDataPlayer::updatePlaybackPosition()
{
    // В режиме реального времени просто увеличиваем позицию
    m_currentPosition += 0.1;
    
    // Проверяем, не достигли ли конца диапазона
    if (m_currentPosition >= m_maxPosition)
    {
        extendTimeRange();
    }
    
    emit currentPositionChanged();
}

void DriverDataPlayer::extendTimeRange()
{
    // Расширяем конечное время на 10 минут
    m_sessionEndTime = m_sessionEndTime.addSecs(60*TIME_RANGE_MINUTES);
    
    // Пересчитываем максимальную позицию
    qint64 durationMs = m_sessionStartTime.msecsTo(m_sessionEndTime);
    m_maxPosition = durationMs / 1000.0;
    
    emit sessionEndTimeChanged();
    emit maxPositionChanged();
    
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
        
        // Вычисляем максимальную позицию
        qint64 durationMs = m_sessionStartTime.msecsTo(m_sessionEndTime);
        m_maxPosition = durationMs / 1000.0;
        
        m_isInitialized = true;
        
        emit sessionStartTimeChanged();
        emit sessionEndTimeChanged();
        emit maxPositionChanged();
        
        qDebug() << "DriverDataPlayer: Initialized time range from" 
                 << m_sessionStartTime.toString() << "to" << m_sessionEndTime.toString();
    }
}
