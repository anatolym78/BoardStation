#ifndef BOARDSTATIONAPP_H
#define BOARDSTATIONAPP_H

#include <QApplication>
#include <QObject>
#include "ViewModel/OutParametersModel.h"
#include "ViewModel/UplinkParametersTreeModel.h"
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
//#include "ViewModel/ChartViewModel.h"
#include "ViewModel/ChatViewGridModel.h"
#include "ViewModel/DataPlayer.h"
#include "ViewModel/SessionPlayer.h"
#include "ViewModel/DriverDataPlayer.h"
#include "Model/DriverAdapter.h"
#include "ViewModel/LiveSession.h"

#include "ViewModel/SimpleTreeModel.h"

class BoardParametersJsonParserNew;

class BoardStationApp : public QApplication
{
	Q_OBJECT

public:
	BoardStationApp(int &argc, char **argv);
	~BoardStationApp();

	// Сохранение живых данных в базу
	bool saveLiveData();
	
	// Отправка параметров на борт
	void sendParametersToBoard();
	
	// Метод для корректного закрытия приложения
	void close();

public:	   
	SessionsListModel* sessionsModel() const { return m_sessionsListModel; }

	UplinkParametersTreeModel* getUplinkParametersModel() const { return m_uplinkParametersModel; }
	
	DebugViewModel* getDebugViewModel() const { return m_debugViewModel; }
	  
	BoardMessagesSqliteWriter* getBoardMessagesWriter() const { return m_boardMessagesWriter;}

	BoardMessagesSqliteReader* getBoardMessagesReader() const { return m_boardMessagesReader; }
	
	drv::IDriver* getDriver() const { return m_driver; }
	DriverAdapter* getDriverAdapter() const { return m_driverAdapter; }

	LiveSession* liveSession() const { return m_sessionsListModel->liveSession(); }
   
private:
	drv::IDriver *m_driver;
	DriverAdapter *m_driverAdapter;
	SessionsListModel *m_sessionsListModel;

	UplinkParametersTreeModel *m_uplinkParametersModel;
	DebugViewModel *m_debugViewModel;

	BoardMessagesSqliteWriter* m_boardMessagesWriter;
	BoardMessagesSqliteReader* m_boardMessagesReader;

	SimpleTreeModel* m_simpleTreeModel;

private:
	void loadUplinkParameters();
	void connectSignals();
};

#endif // BOARDSTATIONAPP_H
