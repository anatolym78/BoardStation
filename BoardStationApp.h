#ifndef BOARDSTATIONAPP_H
#define BOARDSTATIONAPP_H

#include <QApplication>
#include <QObject>
#include "ViewModel/BoardParametersModel.h"
#include "ViewModel/OutParametersModel.h"
#include "ViewModel/UplinkParametersModel.h"
#include "ViewModel/DebugViewModel.h"
#include "ViewModel/ChartSeriesModel.h"
#include "Model/IDriver.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/Parameters.h"
#include "Model/Parameters/OutParametersStorage.h"
#include "Model/Parameters/UplinkParametersParser.h"
#include "Model/Parameters/BasicUplinkParameter.h"
#include "Model/Parameters/BoardMessagesSqliteWriter.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "Interface/Charts/ChartBuilder.h"
#include "ViewModel/BoardParametersListModel.h"

class MainWindow;
class BoardParametersJsonParserNew;

class BoardStationApp : public QApplication
{
    Q_OBJECT

public:
    BoardStationApp(int &argc, char **argv);
    ~BoardStationApp();

    // Методы для работы с главным окном
    void setMainWindow(MainWindow *mainWindow);
    MainWindow* getMainWindow() const;

    // Методы для работы с моделью параметров
    BoardParametersListModel* getParametersModel() const;
    BoardParameterHistoryStorage* getParametersStorage() const;
    
    // Методы для работы с моделью исходящих параметров
    OutParametersModel* getOutParametersModel() const;
    
    // Методы для работы с моделью uplink параметров
    UplinkParametersModel* getUplinkParametersModel() const;
    
    // Методы для работы с моделью отладки
    DebugViewModel* getDebugViewModel() const;
    
    // Методы для работы с моделью серий графиков
    ChartSeriesModel* getChartSeriesModel() const;
    
    // Методы для работы с исходящими параметрами
    void loadOutParameters() const;
    OutParametersStorage* getOutParametersStorage() const;
    
    // Методы для работы с новыми uplink параметрами
    void loadUplinkParameters() const;
    QList<BasicUplinkParameter*> getUplinkParameters() const;
    
    // Методы для работы с записью сообщений от борта
    BoardMessagesSqliteWriter* getBoardMessagesWriter() const;
    BoardMessagesSqliteReader* getBoardMessagesReader() const;
    QString getDatabasePath() const;
    
    // Методы для работы с драйвером
    drv::IDriver* getDriver() const;
    
    // Методы управления прослушиванием
    void startListening();
    void stopListening();
    bool isListening() const;
    
    // Отправка параметров на борт
    void sendParametersToBoard();
    void sendSingleParameter(BasicUplinkParameter* parameter);

private slots:
    void onDataAvailable() const;
    void onParameterChanged(BasicUplinkParameter* parameter);

private:
    void setupDriver();
    void connectSignals();

private:
    MainWindow *m_mainWindow;
    BoardParameterHistoryStorage *m_parametersStorage;
    //BoardParametersModel *m_parametersModel;
    BoardParametersListModel* m_parametersModel;
    OutParametersModel *m_outParametersModel;
    OutParametersStorage *m_outParametersStorage;
    UplinkParametersModel *m_uplinkParametersModel;
    DebugViewModel *m_debugViewModel;
    mutable QList<BasicUplinkParameter*> m_uplinkParameters;
    ChartSeriesModel *m_chartSeriesModel;
    drv::IDriver *m_driver;
    BoardParametersJsonParserNew *m_jsonReader;
    BoardMessagesSqliteWriter *m_boardMessagesWriter;
    BoardMessagesSqliteReader *m_boardMessagesReader;
};

#endif // BOARDSTATIONAPP_H
