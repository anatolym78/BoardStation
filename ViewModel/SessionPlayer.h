#ifndef SESSIONPLAYER_H
#define SESSIONPLAYER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "DataPlayer.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"

class SessionPlayer : public DataPlayer
{
    Q_OBJECT

public:
    explicit SessionPlayer(QObject *parent = nullptr);
    ~SessionPlayer();

    // Методы управления - наследуем базовую логику
    // Методы для работы с хранилищем
    void setStorage(BoardParameterHistoryStorage* storage) override;
    void setReader(BoardMessagesSqliteReader* reader);
    void resetState() override {}
	void initialPlay() override;
	Q_INVOKABLE void moveToBegin() override;

    // Метод для инициализации плеера с уже загруженными данными
    void initializeWithLoadedData();

private:
    void updatePlaybackPosition() override;

private slots:
    void onSessionDataLoaded(int sessionId);

private:
    BoardMessagesSqliteReader* m_reader;
    int m_currentSessionId;
    int m_lastPlayedIndex; // Индекс последнего проигранного параметра
    QDateTime m_lastPlayedPosition; // Последняя проигранная позиция для отслеживания временного интервала
};

#endif // SESSIONPLAYER_H
