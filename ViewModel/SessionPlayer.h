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

private slots:
    void onSessionDataLoaded(int sessionId);

private:
    BoardMessagesSqliteReader* m_reader;
    int m_currentSessionId;
    int m_lastPlayedIndex; // Индекс последнего проигранного параметра
};

#endif // SESSIONPLAYER_H
