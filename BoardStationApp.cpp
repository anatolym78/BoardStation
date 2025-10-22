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

BoardStationApp::BoardStationApp(int &argc, char **argv)
	: QApplication(argc, argv)
	, m_driver(nullptr)
	, m_driverAdapter(nullptr)
	, m_boardMessagesWriter(new BoardMessagesSqliteWriter("BoardStationData.db", this))
	, m_boardMessagesReader(new BoardMessagesSqliteReader("BoardStationData.db", this))
{
	m_driver = new drv::BoardDataEmulator(this);

	m_sessionsListModel = new SessionsListModel(this);
	m_sessionsListModel->setReader(m_boardMessagesReader);
	
	m_driverAdapter = new DriverAdapter(m_driver, this); 
			 
	m_uplinkParametersModel = new UplinkParametersModel(this);
	loadUplinkParameters();

	m_debugViewModel = new DebugViewModel(this);

	connectSignals(); 
}

void BoardStationApp::connectSignals()
{
	//connect(m_driverAdapter, &DriverAdapter::parameterReceived,
	//    liveSession()->getStorage(), &BoardParameterHistoryStorage::addParameter);

	connect(m_uplinkParametersModel, &UplinkParametersModel::parameterChanged,
		m_driverAdapter, &DriverAdapter::sendParameter);

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


BoardParametersListModel* BoardStationApp::parametersModel() const
{
	auto currentSession = m_sessionsListModel->currentSession();
	if (currentSession == nullptr) return nullptr;

	return currentSession->parametersModel();
}


ChatViewGridModel* BoardStationApp::getChartViewModel() const
{
	auto currentSession = m_sessionsListModel->currentSession();
	if (currentSession == nullptr) return nullptr;

	return currentSession->chartsModel();
}

DataPlayer* BoardStationApp::player() const
{
	auto currentSession = m_sessionsListModel->currentSession();
	if (currentSession == nullptr) return nullptr;

	return currentSession->player();
}

bool BoardStationApp::saveLiveData()
{
	qDebug() << "BoardStationApp: Starting to save live data to database";
	
	auto liveSession = m_sessionsListModel->liveSession();
	if (!liveSession || !liveSession->getStorage() || !m_boardMessagesWriter || !m_boardMessagesReader)
	{
		qWarning() << "BoardStationApp: Required components not available for saving live data";
		return false;
	}
	
	// Получаем все параметры из хранилища живой сессии
	QList<BoardParameterSingle*> liveParameters = liveSession->getStorage()->getSessionParameters();
	
	if (liveParameters.isEmpty())
	{
		qWarning() << "BoardStationApp: No live data to save";
		return false;
	}
	
	qDebug() << "BoardStationApp: Found" << liveParameters.size() << "parameters to save";
	
	// Создаем новую сессию для сохранения живых данных
	QString sessionName = QString("Live Session %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	m_boardMessagesWriter->createNewSession(sessionName);
	
	int newSessionId = m_boardMessagesWriter->getCurrentSessionId();
	if (newSessionId <= 0)
	{
		qWarning() << "BoardStationApp: Failed to create new session for live data";
		return false;
	}
	
	qDebug() << "BoardStationApp: Created new session" << newSessionId << "with name" << sessionName;
	
	// Группируем параметры по временным меткам для записи как сообщения
	QMap<QDateTime, QList<BoardParameterSingle*>> parametersByTimestamp;
	
	for (BoardParameterSingle* param : liveParameters)
	{
		if (param)
		{
			parametersByTimestamp[param->timestamp()].append(param);
		}
	}
	
	// Записываем параметры в базу данных группами по времени
	int messagesWritten = 0;
	for (auto it = parametersByTimestamp.begin(); it != parametersByTimestamp.end(); ++it)
	{
		const QDateTime& timestamp = it.key();
		const QList<BoardParameterSingle*>& params = it.value();
		
		// Добавляем сообщение в очередь записи
		m_boardMessagesWriter->addMessage(params, timestamp);
		messagesWritten++;
	}
	
	// Принудительно записываем все данные из очереди
	m_boardMessagesWriter->flushQueue();
	
	qDebug() << "BoardStationApp: Written" << messagesWritten << "messages to database";
	
	// Получаем информацию о созданной сессии из базы данных
	BoardMessagesSqliteReader::SessionInfo sessionInfo = m_boardMessagesReader->getSessionInfo(newSessionId);
	
	// Сбрасываем счетчики живой сессии ПЕРЕД добавлением новой сессии
	if (m_sessionsListModel)
	{
		m_sessionsListModel->resetLiveSessionCounters();
		qDebug() << "BoardStationApp: Reset live session counters";
	}
	
	// Добавляем новую сессию в модель списка сессий
	if (m_sessionsListModel)
	{
		m_sessionsListModel->addRecordedSession(sessionInfo);
		qDebug() << "BoardStationApp: Added new session to sessions list model";
	}
	
	// Очищаем хранилище живой сессии
	liveSession->clearStorage();
	qDebug() << "BoardStationApp: Cleared live session storage";
	
	liveSession->player()->resetState();

	return true;
}

void BoardStationApp::loadUplinkParameters() const
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
	
	// Создаем парсер новых параметров
	UplinkParametersParser parser;
	
	// Парсим параметры
	QList<BasicUplinkParameter*> parsedUplinkParameters = parser.parseParameters(parametersArray);
	
	// Заполняем модель uplink параметров
	if (m_uplinkParametersModel)
	{
		m_uplinkParametersModel->setParameters(parsedUplinkParameters);
	}
}

void BoardStationApp::sendParametersToBoard()
{    
	if (!m_uplinkParametersModel) 
	{
		qWarning() << "BoardStationApp: UplinkParametersModel is not available";
		return;
	}
	
	// Получаем все uplink параметры из модели
	QList<BasicUplinkParameter*> uplinkParameters = m_uplinkParametersModel->getParameters();
	
	if (uplinkParameters.isEmpty()) 
	{
		return;
	}
	
	// Создаем JSON массив из параметров в формате для исходящих параметров
	QJsonArray parametersArray;
	for (BasicUplinkParameter *param : uplinkParameters) 
	{
		if (param && param->isValid())
		{
			QJsonObject paramObj;
			paramObj["label"] = param->getLabel();
			paramObj["value"] = QJsonValue::fromVariant(param->getValue());
			
			parametersArray.append(paramObj);
		}
	}
	
	if (parametersArray.isEmpty())
	{
		return;
	}
	
	// Создаем JSON документ
	QJsonDocument doc(parametersArray);
	QString jsonString = doc.toJson(QJsonDocument::Indented);
		
	// Отправляем данные через драйвер
	if (m_driver) 
	{
		std::string data = jsonString.toStdString();
		m_driver->write(data);
	}
	else 
	{
		qWarning() << "BoardStationApp: Driver is not available";
	}
}

BoardStationApp::~BoardStationApp()
{
}
