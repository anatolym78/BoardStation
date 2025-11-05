#ifndef DRIVERADAPTER_H
#define DRIVERADAPTER_H

#include <QObject>
#include "IDriver.h"
#include "./Parameters/BoardParameterSingle.h"
#include "./Parameters/BasicUplinkParameter.h"
#include "Parameters/Tree/ParameterTreeStorage.h"

class BoardParametersJsonParserNew;
class ParameterTreeJsonParser;

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
    
    // Метод для отправки параметра на борт
    void sendParameter(BasicUplinkParameter* parameter);

signals:
    // Сигнал для каждого отдельного параметра
    void parameterReceived(BoardParameterSingle* parameter);
    
    // Сигнал для группы параметров (если нужно)
    void parametersReceived(const QList<BoardParameterSingle*>& parameters);

    // Сигнал для древовидных параметров
    void parameterTreeReceived(ParameterTreeStorage* root);

private slots:
    void onDriverDataAvailable();

private:
    void createParameters(const QString& data);
    void createTreeParameters(const QString& data);

    void connectToDriver();
    void disconnectFromDriver();

private:
    drv::IDriver* m_driver;
    BoardParametersJsonParserNew* m_jsonParser;
    ParameterTreeJsonParser* m_treeJsonParser;
    bool m_isConnected;
};

#endif // DRIVERADAPTER_H
