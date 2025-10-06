#ifndef DATAPLAYER_H
#define DATAPLAYER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "Model/Parameters/BoardParameterHistoryStorage.h"

class DataPlayer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(double currentPosition READ currentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(double maxPosition READ maxPosition NOTIFY maxPositionChanged)
    Q_PROPERTY(QString currentSessionName READ currentSessionName NOTIFY currentSessionNameChanged)
    Q_PROPERTY(QDateTime sessionStartTime READ sessionStartTime NOTIFY sessionStartTimeChanged)
    Q_PROPERTY(QDateTime sessionEndTime READ sessionEndTime NOTIFY sessionEndTimeChanged)

public:
    explicit DataPlayer(QObject *parent = nullptr);
    virtual ~DataPlayer();

    // Свойства
    bool isPlaying() const { return m_isPlaying; }
    double currentPosition() const { return m_currentPosition; }
    double maxPosition() const { return m_maxPosition; }
    QString currentSessionName() const { return m_currentSessionName; }
    QDateTime sessionStartTime() const { return m_sessionStartTime; }
    QDateTime sessionEndTime() const { return m_sessionEndTime; }

    // Методы управления
    Q_INVOKABLE virtual void play();
    Q_INVOKABLE virtual void stop();
    Q_INVOKABLE virtual void pause();
    Q_INVOKABLE virtual void setPosition(double position);
    
    // Методы для работы с хранилищем
    virtual void setStorage(BoardParameterHistoryStorage* storage);

signals:
    void isPlayingChanged();
    void currentPositionChanged();
    void maxPositionChanged();
    void currentSessionNameChanged();
    void sessionStartTimeChanged();
    void sessionEndTimeChanged();
    void playbackFinished();
    void parameterPlayed(BoardParameterSingle* parameter);

protected slots:
    virtual void onPlaybackTimer();

protected:
    virtual void updatePlaybackPosition();
    virtual void clearStorage();
    QDateTime getCurrentTimeFromPosition() const;

protected:
    BoardParameterHistoryStorage* m_storage;
    QTimer* m_playbackTimer;
    
    bool m_isPlaying;
    double m_currentPosition; // Позиция в секундах от начала сессии
    double m_maxPosition;     // Общая длительность сессии в секундах
    
    QString m_currentSessionName;
    QDateTime m_sessionStartTime;
    QDateTime m_sessionEndTime;
};

#endif // DATAPLAYER_H
