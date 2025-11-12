#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/Parameters/BoardParametersJsonParserNew.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/AppConfigurationReader.h"
#include "Model/Parameters/UplinkParametersParser.h"
#include "Model/Parameters/BoolUplinkParameter.h"
#include "Model/DriverAdapter.h"
#include "ViewModel/RecordedSession.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include "ViewModel/LiveSession.h"
#include "Model/Parameters/Tree/ParameterTreeStorage.h"
#include "Model/Parameters/ParameterTreeJsonParser.h"


BoardStationApp::BoardStationApp(int &argc, char **argv)
	: QApplication(argc, argv)
	, m_driver(nullptr)
	, m_driverAdapter(nullptr)
	, m_boardMessagesWriter(new BoardMessagesSqliteWriter("BoardStationData.db", this))
	, m_boardMessagesReader(new BoardMessagesSqliteReader("BoardStationData.db", this))
{
	// test
	m_simpleTreeModel = new SimpleTreeModel(this);
	// test

	m_driver = new drv::BoardDataEmulator(this);

	m_sessionsListModel = new SessionsListModel(this);
	m_sessionsListModel->setReader(m_boardMessagesReader);
	
	m_driverAdapter = new DriverAdapter(m_driver, this); 
			 
	m_uplinkParametersModel = new UplinkParametersTreeModel(this);
	loadUplinkParameters();

	m_debugViewModel = new DebugViewModel(this);

	connectSignals(); 
}

void BoardStationApp::connectSignals()
{
	if (liveSession())
	{
		//connect(m_driverAdapter, &DriverAdapter::parameterTreeReceived,
		//	liveSession()->storage(), &ParameterTreeStorage::appendSnapshot);

		connect(m_boardMessagesWriter, &BoardMessagesSqliteWriter::writeSuccess,
			liveSession(), &LiveSession::incrementMessageCount);

	}

    // connect(m_uplinkParametersModel, &UplinkParametersModel::parameterChanged,
    // 	m_driverAdapter, &DriverAdapter::sendParameter);

	connect(this, &QApplication::aboutToQuit, this, &BoardStationApp::close);
}

void BoardStationApp::close()
{
	for (auto i = 0; i < m_sessionsListModel->rowCount(); i++)
	{
		auto session = m_sessionsListModel->getSession(i);
		session->player()->stop();
	}
}

bool BoardStationApp::saveLiveData()
{
	if (!liveSession()) return false;

	qDebug() << "BoardStationApp: Starting to save live tree data to database";

	auto live = m_sessionsListModel ? m_sessionsListModel->liveSession() : nullptr;
	if (!live || !m_boardMessagesWriter)
	{
		qWarning() << "BoardStationApp: Required components not available for saving live data";
		return false;
	}

	// Создаём новую сессию
	QString sessionName = QString("Live Session %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	m_boardMessagesWriter->createNewSession(sessionName);

	int newSessionId = m_boardMessagesWriter->getCurrentSessionId();
	if (newSessionId <= 0)
	{
		qWarning() << "BoardStationApp: Failed to create new session for live data";
		return false;
	}

	// Записываем древовидную модель параметров
	if (!m_boardMessagesWriter->writeTree(live->storage()))
	{
		qWarning() << "BoardStationApp: Failed to write tree data";
		return false;
	}

	qDebug() << "BoardStationApp: Live tree data saved to session" << newSessionId;
	return true;
}

void BoardStationApp::loadUplinkParameters()
{
	qDebug() << "BoardStationApp: Loading new uplink parameters from configuration.json";
	
	// Создаем читатель конфигурации
	AppConfigurationReader reader;
	
	// Формируем полный путь к файлу конфигурации
	QString configPath = QApplication::applicationDirPath() + "/configuration.json";
	qDebug() << "BoardStationApp: Configuration path:" << configPath;
	
	// Загружаем конфигурацию
	if (!reader.loadConfiguration(configPath))
	{
		qWarning() << "BoardStationApp: Failed to load configuration from" << configPath;
		return;
	}
	
	// Получаем узел с параметрами
	QJsonArray parametersArray = reader.getParametersNode();
	if (parametersArray.isEmpty())
	{
		qWarning() << "BoardStationApp: Parameters node is empty or not found";
		return;
	}
	
	// Создаем парсер новых параметров для ParameterTreeStorage
	ParameterTreeJsonParser parser(this);
	
	// Создаем хранилище для uplink параметров
	ParameterTreeStorage* uplinkParametersStorage = new ParameterTreeStorage(this);
	
	// Парсим параметры в ParameterTreeStorage
	parser.updateJsonFromArray(parametersArray, uplinkParametersStorage);
	
	if (!parser.getLastError().isEmpty())
	{
		qWarning() << "BoardStationApp: Failed to parse uplink parameters. Error:" << parser.getLastError();
		return;
	}
	
	qDebug() << "BoardStationApp: Successfully loaded" << uplinkParametersStorage->childCount() << "top-level parameter groups";
	
	// Устанавливаем параметры в модель
	if (m_uplinkParametersModel)
	{
		m_uplinkParametersModel->setSnapshot(uplinkParametersStorage, false);
		qDebug() << "BoardStationApp: Parameters set to model";
	}
}

void BoardStationApp::sendParametersToBoard()
{    
    // if (!m_uplinkParametersModel)
    // {
    // 	qWarning() << "BoardStationApp: UplinkParametersModel is not available";
    // 	return;
    // }
	
    // // Получаем все uplink параметры из модели
    // QList<BasicUplinkParameter*> uplinkParameters = m_uplinkParametersModel->getParameters();
	
    // if (uplinkParameters.isEmpty())
    // {
    // 	return;
    // }
	
    // // Создаем JSON массив из параметров в формате для исходящих параметров
    // QJsonArray parametersArray;
    // for (BasicUplinkParameter *param : uplinkParameters)
    // {
    // 	if (param && param->isValid())
    // 	{
    // 		QJsonObject paramObj;
    // 		paramObj["label"] = param->getLabel();
    // 		paramObj["value"] = QJsonValue::fromVariant(param->getValue());
			
    // 		parametersArray.append(paramObj);
    // 	}
    // }
	
    // if (parametersArray.isEmpty())
    // {
    // 	return;
    // }
	
    // // Создаем JSON документ
    // QJsonDocument doc(parametersArray);
    // QString jsonString = doc.toJson(QJsonDocument::Indented);
		
    // // Отправляем данные через драйвер
    // if (m_driver)
    // {
    // 	std::string data = jsonString.toStdString();
    // 	m_driver->write(data);
    // }
    // else
    // {
    // 	qWarning() << "BoardStationApp: Driver is not available";
    // }
}

BoardStationApp::~BoardStationApp()
{
}
