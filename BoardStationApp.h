#ifndef BOARDSTATIONAPP_H
#define BOARDSTATIONAPP_H

#include <QApplication>
#include <QObject>
#include "ViewModel/OutParametersModel.h"
#include "ViewModel/UplinkParametersModel.h"
#include "ViewModel/DebugViewModel.h"
#include "Model/IDriver.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/Parameters.h"
#include "Model/Parameters/UplinkParametersParser.h"
#include "Model/Parameters/BasicUplinkParameter.h"
#include "Model/Parameters/BoardMessagesSqliteWriter.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "ViewModel/BoardParametersListModel.h"
#include "ViewModel/SessionsListModel.h"
#include "ViewModel/ChartViewModel.h"
#include "ViewModel/DataPlayer.h"
#include "ViewModel/SessionPlayer.h"
#include "ViewModel/DriverDataPlayer.h"
#include "Model/DriverAdapter.h"
#include "ViewModel/LiveSession.h"

class BoardParametersJsonParserNew;

class BoardStationApp : public QApplication
{
    Q_OBJECT

public:
    BoardStationApp(int &argc, char **argv);
    ~BoardStationApp();


    // Методы для работы с моделью параметров
    BoardParametersListModel* getParametersModel() const { return m_parametersModel; }
    ChartViewModel* getChartViewModel() const { return m_chatsViewModel; }
       
    // Методы для работы с моделью uplink параметров
    UplinkParametersModel* getUplinkParametersModel() const { return m_uplinkParametersModel; }
    
    // Методы для работы с моделью отладки
    DebugViewModel* getDebugViewModel() const { return m_debugViewModel; }
    
    // Методы для работы с моделью сессий
    SessionsListModel* getSessionsListModel() const { return m_sessionsListModel; }
       
    // Методы для работы с новыми uplink параметрами
    void loadUplinkParameters() const;
    
    // Методы для работы с записью сообщений от борта

	BoardMessagesSqliteWriter* getBoardMessagesWriter() const
	{
		return m_boardMessagesWriter;
	}

    BoardMessagesSqliteReader* getBoardMessagesReader() const { return m_boardMessagesReader; }
    QString getDatabasePath() const;
    
    // Методы для работы с драйвером
    drv::IDriver* getDriver() const { return m_driver; }
    DriverAdapter* getDriverAdapter() const { return m_driverAdapter; }

    // Методы для работы с сессиями
    LiveSession* getLiveSession() const { return m_liveSession; }

    DataPlayer* getDataPlayer() const { return m_dataPlayer; }
    
    // Сохранение живых данных в базу
    bool saveLiveData();
    
    // Метод для загрузки сессии в плеер
    void loadSession(int sessionId);
    
    // Метод для переключения на сессию
    void switchToSession(int sessionIndex);
    void switchToLiveSession();
      
    // Отправка параметров на борт
    void sendParametersToBoard();
    void sendSingleParameter(BasicUplinkParameter* parameter);

private slots:
    void onParameterChanged(BasicUplinkParameter* parameter);

private:
    void setupDriver();
    void connectSignals();

private:
    drv::IDriver *m_driver;
    DriverAdapter *m_driverAdapter;
    SessionsListModel *m_sessionsListModel;
    LiveSession *m_liveSession;
    DataPlayer* m_dataPlayer;
	BoardParametersListModel* m_parametersModel;
	ChartViewModel* m_chatsViewModel;


    UplinkParametersModel *m_uplinkParametersModel;
    DebugViewModel *m_debugViewModel;

	BoardMessagesSqliteWriter* m_boardMessagesWriter;
	BoardMessagesSqliteReader* m_boardMessagesReader;
};

#endif // BOARDSTATIONAPP_H
