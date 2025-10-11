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
    , m_parametersModel(nullptr)
    , m_driver(nullptr)
    , m_driverAdapter(nullptr)
    , m_liveSession(nullptr)
    , m_boardMessagesWriter(new BoardMessagesSqliteWriter("BoardStationData.db", this))
    , m_boardMessagesReader(new BoardMessagesSqliteReader("BoardStationData.db", this))
    , m_dataPlayer(new DriverDataPlayer(this))// По умолчанию используем DriverDataPlayer
{
    // Создаем живую сессию (она содержит свое хранилище)
    m_liveSession = new LiveSession(this);

    // Создаем модель параметров
	m_parametersModel = new BoardParametersListModel(m_boardMessagesReader, this);

    // Создаем модель списка чартов
    m_chatsViewModel = new ChartViewModel(this);
       
    // Настраиваем DriverDataPlayer с хранилищем из живой сессии
    m_dataPlayer->setStorage(m_liveSession->getStorage());
        
    // Создаем модель uplink параметров
    m_uplinkParametersModel = new UplinkParametersModel(this);
    
    // Подключаем сигнал изменения параметра к слоту
    connect(m_uplinkParametersModel, &UplinkParametersModel::parameterChanged,
            this, &BoardStationApp::onParameterChanged);
    
    // Создаем модель отладки
    m_debugViewModel = new DebugViewModel(this);
        
    // Создаем модель списка сессий
    m_sessionsListModel = new SessionsListModel(this);
    m_sessionsListModel->setReader(m_boardMessagesReader);
    
    // Загружаем новые uplink параметры
    loadUplinkParameters();
    
    // Очищаем данные текущей сессии (вместо очистки файла)
    //m_boardMessagesWriter->clearCurrentSession();
    
    // НЕ создаём сессию при старте - она будет создана при включении переключателя записи
    
    // Настраиваем драйвер
    setupDriver();
    
    // Подключаем сигналы
    connectSignals(); 
}

BoardStationApp::~BoardStationApp()
{
    qDebug() << "BoardStationApp: Application shutdown - сохранение данных...";
    
    // Принудительно сохраняем все данные перед закрытием
    if (m_boardMessagesWriter) 
    {
        //m_boardMessagesWriter->forceSave();
    }

    m_dataPlayer->stop();
    
    //qDebug() << "BoardStationApp: Application shutdown";
}

void BoardStationApp::setupDriver()
{
    // Создаем эмулятор данных платы и присваиваем его интерфейсу
    m_driver = new drv::BoardDataEmulator(this);
    
    // Создаем адаптер драйвера (он автоматически запустит прослушивание)
    m_driverAdapter = new DriverAdapter(m_driver, this);
    
    //qDebug() << "BoardStationApp: Driver configured and listening started";
}

void BoardStationApp::connectSignals()
{
    // Подключаем адаптер драйвера к хранилищу живой сессии
    if (m_driverAdapter && m_liveSession && m_liveSession->getStorage())
    {
        connect(m_driverAdapter, &DriverAdapter::parameterReceived,
                m_liveSession->getStorage(), &BoardParameterHistoryStorage::addParameter);
    }

    // Подключаем плеер к сигналам параметров из хранилища живой сессии
    if (m_dataPlayer && m_liveSession && m_liveSession->getStorage())
    {
        connect(m_liveSession->getStorage(), &BoardParameterHistoryStorage::parameterEmitted,
                m_dataPlayer, &DataPlayer::onParameterReceived);
    }
    
    // Подключаем плеер к модели параметров
    if (m_dataPlayer && m_parametersModel)
    {
        connect(m_dataPlayer, &DataPlayer::parameterPlayed, 
                m_parametersModel, &BoardParametersListModel::onNewParameterAdded);
    }
    
    // Подключаем сигнал новых параметров к живой сессии для обновления счетчиков
    if (m_sessionsListModel && m_liveSession && m_liveSession->getStorage())
    {
        connect(m_liveSession->getStorage(), &BoardParameterHistoryStorage::newParameterAdded,
                m_sessionsListModel, &SessionsListModel::onNewParameterAdded);
    }
}

QString BoardStationApp::getDatabasePath() const
{
    if (m_boardMessagesWriter) 
    {
        return m_boardMessagesWriter->getDatabasePath();
    }
    return QString();
}

void BoardStationApp::sendParametersToBoard()
{
    //qDebug() << "BoardStationApp: Sending uplink parameters to board";
    
    if (!m_uplinkParametersModel) 
    {
        qWarning() << "BoardStationApp: UplinkParametersModel is not available";
        return;
    }
    
    // Получаем все uplink параметры из модели
    QList<BasicUplinkParameter*> uplinkParameters = m_uplinkParametersModel->getParameters();
    
    if (uplinkParameters.isEmpty()) 
    {
        //qDebug() << "BoardStationApp: No uplink parameters to send";
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
        //qDebug() << "BoardStationApp: No valid uplink parameters to send";
        return;
    }
    
    // Создаем JSON документ
    QJsonDocument doc(parametersArray);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    
    //qDebug() << "BoardStationApp: Sending JSON to board:" << jsonString;
    
    // Отправляем данные через драйвер
    if (m_driver) 
    {
        std::string data = jsonString.toStdString();
        m_driver->write(data);
        //qDebug() << "BoardStationApp: Uplink parameters sent to board successfully";
    }
    else 
    {
        qWarning() << "BoardStationApp: Driver is not available";
    }
}

void BoardStationApp::sendSingleParameter(BasicUplinkParameter* parameter)
{
    if (!parameter || !parameter->isValid())
    {
        qWarning() << "BoardStationApp: Invalid parameter for single send";
        return;
    }
    
    if (!m_driver)
    {
        qWarning() << "BoardStationApp: Driver is not available for single parameter send";
        return;
    }
    
    // Создаем JSON массив с одним параметром
    QJsonArray parametersArray;
    QJsonObject paramObj;
    paramObj["label"] = parameter->getLabel();
    paramObj["value"] = QJsonValue::fromVariant(parameter->getValue());
    
    parametersArray.append(paramObj);
    
    // Создаем JSON документ
    QJsonDocument doc(parametersArray);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    
    // Отправляем данные через драйвер
    std::string data = jsonString.toStdString();
    m_driver->write(data);
    
    qDebug() << "BoardStationApp: Single parameter sent:" << parameter->getLabel() << "=" << parameter->getValue();
}

void BoardStationApp::onParameterChanged(BasicUplinkParameter* parameter)
{
    if (!parameter)
    {
        qWarning() << "BoardStationApp: Received null parameter in onParameterChanged";
        return;
    }
    
    qDebug() << "BoardStationApp: Parameter changed:" << parameter->getLabel() << "=" << parameter->getValue();
    
    // Отправляем только этот параметр
    sendSingleParameter(parameter);
}

bool BoardStationApp::saveLiveData()
{
    qDebug() << "BoardStationApp: Starting to save live data to database";
    
    if (!m_liveSession || !m_liveSession->getStorage() || !m_boardMessagesWriter || !m_boardMessagesReader)
    {
        qWarning() << "BoardStationApp: Required components not available for saving live data";
        return false;
    }
    
    // Получаем все параметры из хранилища живой сессии
    QList<BoardParameterSingle*> liveParameters = m_liveSession->getStorage()->getSessionParameters();
    
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
    m_liveSession->clearStorage();
    qDebug() << "BoardStationApp: Cleared live session storage";
    
    // Сбрасываем состояние DriverDataPlayer
    if (m_dataPlayer)
    {
        DriverDataPlayer* driverPlayer = qobject_cast<DriverDataPlayer*>(m_dataPlayer);
        if (driverPlayer)
        {
            // Сбрасываем состояние плеера
            driverPlayer->resetState();
            qDebug() << "BoardStationApp: Reset DriverDataPlayer state";
        }
    }

	//m_sessionsListModel->refreshSessions();
    
    qDebug() << "BoardStationApp: Successfully saved live data to session" << newSessionId;

    return true;
}

void BoardStationApp::loadSession(int sessionId)
{
    if (m_sessionsListModel)
    {
        // Находим индекс сессии по ID
        int sessionIndex = m_sessionsListModel->findSessionIndex(sessionId);
        if (sessionIndex >= 0)
        {
            switchToSession(sessionIndex);
        }
        else
        {
            qWarning() << "BoardStationApp: Session with ID" << sessionId << "not found";
        }
    }
    else
    {
        qWarning() << "BoardStationApp: SessionsListModel is not available";
    }
}

void BoardStationApp::switchToSession(int sessionIndex)
{
    if (!m_sessionsListModel || !m_dataPlayer)
    {
        qWarning() << "BoardStationApp: SessionsListModel or DataPlayer is not available";
        return;
    }
    
    Session* session = m_sessionsListModel->getSession(sessionIndex);
    if (!session)
    {
        qWarning() << "BoardStationApp: Session at index" << sessionIndex << "not found";
        return;
    }
    
    qDebug() << "BoardStationApp: Switching to session" << session->getName();
    
    // Останавливаем текущее проигрывание
    m_dataPlayer->stop();
    
    // Если это RecordedSession, создаем SessionPlayer
    RecordedSession* recordedSession = qobject_cast<RecordedSession*>(session);
    if (recordedSession)
    {
        // Создаем новый SessionPlayer для записанной сессии
        SessionPlayer* sessionPlayer = new SessionPlayer(this);
        sessionPlayer->setStorage(recordedSession->getStorage());
        sessionPlayer->setReader(m_boardMessagesReader);
        
        // Заменяем текущий плеер
        m_dataPlayer->deleteLater();
        m_dataPlayer = sessionPlayer;
        
        // Загружаем данные из базы
        recordedSession->loadDataFromDatabase(m_boardMessagesReader);
        
        qDebug() << "BoardStationApp: Created SessionPlayer for recorded session";
    }
    else
    {
        // Для других типов сессий просто меняем хранилище
        m_dataPlayer->setStorage(session->getStorage());
    }
    
    qDebug() << "BoardStationApp: Switched to session" << session->getName();
}

void BoardStationApp::switchToLiveSession()
{
    if (!m_liveSession || !m_dataPlayer)
    {
        qWarning() << "BoardStationApp: LiveSession or DataPlayer is not available";
        return;
    }
    
    qDebug() << "BoardStationApp: Switching to live session";
    
    // Останавливаем текущее проигрывание
    m_dataPlayer->stop();
    
    // Если текущий плеер не DriverDataPlayer, создаем новый
    DriverDataPlayer* driverPlayer = qobject_cast<DriverDataPlayer*>(m_dataPlayer);
    if (!driverPlayer)
    {
        // Создаем новый DriverDataPlayer для живой сессии
        DriverDataPlayer* newDriverPlayer = new DriverDataPlayer(this);
        newDriverPlayer->setStorage(m_liveSession->getStorage());
        
        // Заменяем текущий плеер
        m_dataPlayer->deleteLater();
        m_dataPlayer = newDriverPlayer;
        
        qDebug() << "BoardStationApp: Created DriverDataPlayer for live session";
    }
    else
    {
        // Просто меняем хранилище
        m_dataPlayer->setStorage(m_liveSession->getStorage());
    }
    
    // Для живой сессии автоматически запускаем проигрывание
    m_dataPlayer->play();
    
    qDebug() << "BoardStationApp: Switched to live session and started playback";
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
    
    qDebug() << "BoardStationApp: Successfully loaded" << parsedUplinkParameters.size() << "uplink parameters";
    
    // Выводим информацию о загруженных параметрах для отладки
    for (BasicUplinkParameter *param : parsedUplinkParameters)
    {
        if (param)
        {
            qDebug() << "Uplink Parameter:" << param->getLabel() 
                     << "Type:" << param->getControlType()
                     << "Value:" << param->getValue()
                     << "IsInt:" << param->isIntParameter()
                     << "IsDouble:" << param->isDoubleParameter()
                     << "IsString:" << param->isStringParameter()
                     << "IsBool:" << param->isBoolParameter()
                     << "HasRange:" << param->hasRange()
                     << "HasStep:" << param->hasStep()
                     << "IsList:" << param->isListParameter();
        }
    }
}