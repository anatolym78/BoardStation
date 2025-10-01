#ifndef SESSIONPLAYER_H
#define SESSIONPLAYER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"

class SessionPlayer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(double currentPosition READ currentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(double maxPosition READ maxPosition NOTIFY maxPositionChanged)
    Q_PROPERTY(QString currentSessionName READ currentSessionName NOTIFY currentSessionNameChanged)
    Q_PROPERTY(QDateTime sessionStartTime READ sessionStartTime NOTIFY sessionStartTimeChanged)
    Q_PROPERTY(QDateTime sessionEndTime READ sessionEndTime NOTIFY sessionEndTimeChanged)

public:
    explicit SessionPlayer(QObject *parent = nullptr);
    ~SessionPlayer();

    // Свойства
    bool isPlaying() const { return m_isPlaying; }
    double currentPosition() const { return m_currentPosition; }
    double maxPosition() const { return m_maxPosition; }
    QString currentSessionName() const { return m_currentSessionName; }
    QDateTime sessionStartTime() const { return m_sessionStartTime; }
    QDateTime sessionEndTime() const { return m_sessionEndTime; }

    // Методы управления
    Q_INVOKABLE void loadSession(int sessionId);
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void setPosition(double position);
    
    // Методы для работы с хранилищем
    void setReader(BoardMessagesSqliteReader* reader);
    void setStorage(BoardParameterHistoryStorage* storage);

signals:
    void isPlayingChanged();
    void currentPositionChanged();
    void maxPositionChanged();
    void currentSessionNameChanged();
    void sessionStartTimeChanged();
    void sessionEndTimeChanged();
    void sessionLoaded(int sessionId, const QString& sessionName);
    void playbackFinished();

private slots:
    void onPlaybackTimer();

private:
    void updatePlaybackPosition();
    void loadSessionData(int sessionId);
    void clearStorage();

private:
    BoardMessagesSqliteReader* m_reader;
    BoardParameterHistoryStorage* m_storage;
    QTimer* m_playbackTimer;
    
    bool m_isPlaying;
    double m_currentPosition; // Позиция в секундах от начала сессии
    double m_maxPosition;     // Общая длительность сессии в секундах
    
    QString m_currentSessionName;
    QDateTime m_sessionStartTime;
    QDateTime m_sessionEndTime;
    
    int m_currentSessionId;
    QList<BoardParameterSingle*> m_sessionData;
    int m_currentDataIndex;
};

#endif // SESSIONPLAYER_H
