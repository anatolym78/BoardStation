#ifndef BOARDSTATIONAPP_H
#define BOARDSTATIONAPP_H

#include <QApplication>
#include <QObject>
#include "ViewModel/BoardParametersModel.h"
#include "ViewModel/OutParametersModel.h"
#include "ViewModel/ChartSeriesModel.h"
#include "Model/IDriver.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/Parameters.h"
#include "Model/Parameters/OutParametersStorage.h"
#include "Model/Parameters/BoardMessagesJsonWriterNew.h"
#include "Interface/Charts/ChartBuilder.h"

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
    BoardParametersModel* getParametersModel() const;
    BoardParameterHistoryStorage* getParametersStorage() const;
    
    // Методы для работы с моделью исходящих параметров
    OutParametersModel* getOutParametersModel() const;
    
    // Методы для работы с моделью серий графиков
    ChartSeriesModel* getChartSeriesModel() const;
    
    // Методы для работы с исходящими параметрами
    void loadOutParameters() const;
    OutParametersStorage* getOutParametersStorage() const;
    
    // Методы для работы с записью сообщений от борта
    BoardMessagesJsonWriterNew* getBoardMessagesWriter() const;
    
    // Отправка параметров на борт
    void sendParametersToBoard();

private slots:
    void onDataAvailable() const;

private:
    void setupDriver();
    void connectSignals();

private:
    MainWindow *m_mainWindow;
    BoardParameterHistoryStorage *m_parametersStorage;
    BoardParametersModel *m_parametersModel;
    OutParametersModel *m_outParametersModel;
    OutParametersStorage *m_outParametersStorage;
    ChartSeriesModel *m_chartSeriesModel;
    drv::IDriver *m_driver;
    BoardParametersJsonParserNew *m_jsonReader;
    BoardMessagesJsonWriterNew *m_boardMessagesWriter;
};

#endif // BOARDSTATIONAPP_H
