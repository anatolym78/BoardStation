#ifndef DRIVERDATAPLAYER_H
#define DRIVERDATAPLAYER_H

#include "DataPlayer.h"

class DriverDataPlayer : public DataPlayer
{
    Q_OBJECT

public:
    explicit DriverDataPlayer(QObject *parent = nullptr);
    ~DriverDataPlayer();

    // Методы управления
    Q_INVOKABLE void play() override;
    Q_INVOKABLE void stop() override;
    Q_INVOKABLE void pause() override;
    Q_INVOKABLE void setPosition(QDateTime position) override;
    
    // Методы для работы с хранилищем
    void setStorage(BoardParameterHistoryStorage* storage) override;

private slots:
    void onNewParameterAdded(BoardParameterSingle* parameter);

private:
    void updatePlaybackPosition() override;
    void extendTimeRange();
    void initializeTimeRange();
    void checkAndPlayParameters();

private:
    bool m_isInitialized;
    static const int TIME_RANGE_MINUTES = 10; // Расширение диапазона на 10 минут
};

#endif // DRIVERDATAPLAYER_H
