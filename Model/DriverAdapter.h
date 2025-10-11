#ifndef DRIVERADAPTER_H
#define DRIVERADAPTER_H

#include <QObject>
#include "IDriver.h"
#include "./Parameters/BoardParameterSingle.h"

class BoardParametersJsonParserNew;

class DriverAdapter : public QObject
{
    Q_OBJECT

public:
    explicit DriverAdapter(drv::IDriver* driver, QObject *parent = nullptr);
    ~DriverAdapter();

    // Методы для работы с драйвером
    void setDriver(drv::IDriver* driver);
    drv::IDriver* getDriver() const;

    // Методы управления прослушиванием
    void startListening();
    void stopListening();
    bool isListening() const;

signals:
    // Сигнал для каждого отдельного параметра
    void parameterReceived(BoardParameterSingle* parameter);
    
    // Сигнал для группы параметров (если нужно)
    void parametersReceived(const QList<BoardParameterSingle*>& parameters);

private slots:
    void onDriverDataAvailable();

private:
    void connectToDriver();
    void disconnectFromDriver();

private:
    drv::IDriver* m_driver;
    BoardParametersJsonParserNew* m_jsonParser;
    bool m_isConnected;
};

#endif // DRIVERADAPTER_H
