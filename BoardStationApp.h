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

class BoardParametersJsonParserNew;

class BoardStationApp : public QApplication
{
    Q_OBJECT

public:
    BoardStationApp(int &argc, char **argv);
    ~BoardStationApp();


    // Методы для работы с моделью параметров
    BoardParametersListModel* getParametersModel() const;
    ChartViewModel* getChartViewModel() const { return m_chatsViewModel; }
    BoardParameterHistoryStorage* getParametersStorage() const;
       
    // Методы для работы с моделью uplink параметров
    UplinkParametersModel* getUplinkParametersModel() const;
    
    // Методы для работы с моделью отладки
    DebugViewModel* getDebugViewModel() const;
    
    // Методы для работы с моделью сессий
    SessionsListModel* getSessionsListModel() const;
       
    // Методы для работы с новыми uplink параметрами
    void loadUplinkParameters() const;
    QList<BasicUplinkParameter*> getUplinkParameters() const;
    
    // Методы для работы с записью сообщений от борта
    BoardMessagesSqliteWriter* getBoardMessagesWriter() const;
    BoardMessagesSqliteReader* getBoardMessagesReader() const;
    QString getDatabasePath() const;
    
    // Методы для работы с драйвером
    drv::IDriver* getDriver() const;

    DataPlayer* getDataPlayer() const { return m_dataPlayer; }
    
    // Методы управления прослушиванием
    void startListening();
    void stopListening();
    bool isListening() const;
    
    // Методы управления записью в базу данных
    void startRecording();
    void stopRecording();
    bool isRecording() const;

    bool saveLiveData();
    
    // Метод для загрузки сессии в плеер
    void loadSession(int sessionId);
      
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
	BoardParameterHistoryStorage* m_parametersStorage;
	BoardParametersListModel* m_parametersModel;
	ChartViewModel* m_chatsViewModel;
    DataPlayer* m_dataPlayer;

	BoardMessagesSqliteWriter* m_boardMessagesWriter;
	BoardMessagesSqliteReader* m_boardMessagesReader;

    UplinkParametersModel *m_uplinkParametersModel;
    DebugViewModel *m_debugViewModel;
    mutable QList<BasicUplinkParameter*> m_uplinkParameters;
    SessionsListModel *m_sessionsListModel;
    drv::IDriver *m_driver;
    BoardParametersJsonParserNew *m_jsonReader;
};

#endif // BOARDSTATIONAPP_H
