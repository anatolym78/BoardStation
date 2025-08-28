#ifndef BOARDSTATIONAPP_H
#define BOARDSTATIONAPP_H

#include <QApplication>
#include <QObject>
#include "ViewModel/ParametersListModel.h"
#include "ViewModel/EnginesModel.h"
#include "Model/IDriver.h"
#include "Model/ParametersStorage.h"

class MainWindow;
class JsonReader;

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
    ParametersListModel* getParametersModel() const;
    ParametersStorage* getParametersStorage() const;
    void addSampleData();
    
    // Методы для работы с моделью двигателей
    EnginesModel* getEnginesModel() const;

private slots:
    void onDataAvailable();

private:
    void setupDriver();
    void connectSignals();

private:
    MainWindow *m_mainWindow;
    ParametersStorage *m_parametersStorage;
    ParametersListModel *m_parametersModel;
    EnginesModel *m_enginesModel;
    drv::IDriver *m_driver;
    JsonReader *m_jsonReader;
};

#endif // BOARDSTATIONAPP_H
