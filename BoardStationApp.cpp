#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/Parameters/BoardParametersJsonParserNew.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/AppConfigurationReader.h"
#include "Model/Parameters/UplinkParametersParser.h"
#include "Model/Parameters/BoolUplinkParameter.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>

BoardStationApp::BoardStationApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_parametersModel(nullptr)
    , m_driver(nullptr)
    , m_jsonReader(new BoardParametersJsonParserNew(this))
    , m_boardMessagesWriter(new BoardMessagesSqliteWriter("BoardStationData.db", this))
    , m_boardMessagesReader(new BoardMessagesSqliteReader("BoardStationData.db", this))
    , m_dataPlayer(new DriverDataPlayer(this))// По умолчанию используем DriverDataPlayer
{
    // Создаем хранилище параметров
    m_parametersStorage = new BoardParameterHistoryStorage(this);

    // Создаем модель параметров
    //m_parametersModel = new BoardParametersListModel(m_parametersStorage, this);
	m_parametersModel = new BoardParametersListModel(m_boardMessagesReader, this);

    // Создаем модель списка чартов
    m_chatsViewModel = new ChartViewModel(this);
       
    // Настраиваем DriverDataPlayer
    m_dataPlayer->setStorage(m_parametersStorage);
        
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
    m_boardMessagesWriter->clearCurrentSession();
    
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

BoardParametersListModel* BoardStationApp::getParametersModel() const
{
    return m_parametersModel;
}

UplinkParametersModel* BoardStationApp::getUplinkParametersModel() const
{
    return m_uplinkParametersModel;
}

DebugViewModel* BoardStationApp::getDebugViewModel() const
{
    return m_debugViewModel;
}

SessionsListModel* BoardStationApp::getSessionsListModel() const
{
    return m_sessionsListModel;
}

BoardParameterHistoryStorage* BoardStationApp::getParametersStorage() const
{
    return m_parametersStorage;
}

void BoardStationApp::setupDriver()
{
    // Создаем эмулятор данных платы и присваиваем его интерфейсу
    m_driver = new drv::BoardDataEmulator(this);
    
    // Автоматически запускаем прослушивание драйвера
    if (m_driver)
    {
        m_driver->startListening();
    }
    
    //qDebug() << "BoardStationApp: Driver configured and listening started";
}

void BoardStationApp::connectSignals()
{
    if (m_driver)
    {
        connect(m_driver, &drv::IDriver::dataAvailable, this, &BoardStationApp::onDataAvailable);
    }

    connect(m_dataPlayer, &DataPlayer::parameterPlayed, m_parametersModel, &BoardParametersListModel::onNewParameterAdded);
    
    // Подключаем сигнал новых параметров к живой сессии для обновления счетчиков
    if (m_sessionsListModel && m_parametersStorage)
    {
        connect(m_parametersStorage, &BoardParameterHistoryStorage::newParameterAdded,
                m_sessionsListModel, &SessionsListModel::onNewParameterAdded);
    }
	
}

void BoardStationApp::onDataAvailable() const
{
    //qDebug() << "BoardStationApp: Received new data from driver";
    
    if (!m_parametersModel || !m_driver)
    {
        return;
    }
    
    // Читаем данные от драйвера
    QString data = QString::fromStdString(m_driver->read());
    //qDebug() << "BoardStationApp: Read data from driver:" << data;
    
    if (data.isEmpty())
    {
        //qDebug() << "BoardStationApp: Driver data is empty";
        return;
    }
    
    // Парсим JSON и получаем отдельные параметры
    QList<BoardParameterSingle*> newParameters = m_jsonReader->parseParametersFromString(data);
    
    if (newParameters.isEmpty())
    {
        //qDebug() << "BoardStationApp: Failed to extract parameters from driver data";
        return;
    }

    m_parametersStorage->addParameters(newParameters);

    // Обновляем счетчики живой сессии
    if (m_sessionsListModel)
    {
        m_sessionsListModel->updateLiveSessionCounters();
    }
}

BoardMessagesSqliteWriter* BoardStationApp::getBoardMessagesWriter() const
{
    return m_boardMessagesWriter;
}

BoardMessagesSqliteReader* BoardStationApp::getBoardMessagesReader() const
{
    return m_boardMessagesReader;
}

QString BoardStationApp::getDatabasePath() const
{
    if (m_boardMessagesWriter) 
    {
        return m_boardMessagesWriter->getDatabasePath();
    }
    return QString();
}

drv::IDriver* BoardStationApp::getDriver() const
{
    return m_driver;
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
    
    // Сохраняем параметры в список
    m_uplinkParameters = parsedUplinkParameters;
    
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

QList<BasicUplinkParameter*> BoardStationApp::getUplinkParameters() const
{
    return m_uplinkParameters;
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

// Методы управления прослушиванием

void BoardStationApp::startListening()
{
    if (m_driver)
    {
        m_driver->startListening();
        qDebug() << "BoardStationApp: Started listening";
    }
}

void BoardStationApp::stopListening()
{
    if (m_driver)
    {
        m_driver->stopListening();
        
        // Очищаем модель параметров
        if (m_parametersModel)
        {
            m_parametersModel->clearParameters();
        }
        
        // Очищаем хранилище параметров
        if (m_parametersStorage)
        {
            m_parametersStorage->clear();
        }
        
        qDebug() << "BoardStationApp: Stopped listening and cleared data";
    }
}

bool BoardStationApp::isListening() const
{
    if (m_driver)
    {
        return m_driver->isListening();
    }
    return false;
}

void BoardStationApp::startRecording()
{
    //if (!m_isRecording)
    //{
    //    m_isRecording = true;
    //    
    //    // Создаём новую сессию для записи
    //    // Получаем следующий номер сессии
    //    int nextSessionNumber = 1;
    //    if (m_sessionsListModel)
    //    {
    //        // Находим максимальный номер сессии и увеличиваем на 1
    //        for (int i = 0; i < m_sessionsListModel->rowCount(); ++i)
    //        {
    //            QString sessionName = m_sessionsListModel->data(m_sessionsListModel->index(i), SessionsListModel::SessionNameRole).toString();
    //            QRegExp rx("Session (\\d+)");
    //            if (rx.indexIn(sessionName) != -1)
    //            {
    //                int sessionNum = rx.cap(1).toInt();
    //                if (sessionNum >= nextSessionNumber)
    //                {
    //                    nextSessionNumber = sessionNum + 1;
    //                }
    //            }
    //        }
    //    }
    //    
    //    QString sessionName = QString("Session %1").arg(nextSessionNumber);
    //    m_boardMessagesWriter->createNewSession(sessionName);
    //    
    //    // Добавляем новую сессию в модель списка
    //    if (m_sessionsListModel && m_boardMessagesReader)
    //    {
    //        // Получаем информацию о только что созданной сессии
    //        int currentSessionId = m_boardMessagesWriter->getCurrentSessionId();
    //        if (currentSessionId > 0)
    //        {
    //            BoardMessagesSqliteReader::SessionInfo sessionInfo = m_boardMessagesReader->getSessionInfo(currentSessionId);
    //            m_sessionsListModel->addRecordedSession(sessionInfo);
    //        }
    //    }

    //    if (m_sessionsListModel)
    //    {
    //        m_sessionsListModel->startRecording();
    //    }
    //    
    //    qDebug() << "BoardStationApp: Started recording to database";
    //}
}

void BoardStationApp::stopRecording()
{
  //  if (m_isRecording)
  //  {
  //      m_isRecording = false;
  //      
  //      // Принудительно сохраняем все данные из очереди
  //      if (m_boardMessagesWriter)
  //      {
  //          m_boardMessagesWriter->flushQueue();
  //      }

		//if (m_sessionsListModel)
		//{
		//	m_sessionsListModel->stopRecording();
		//}
  //      
  //      qDebug() << "BoardStationApp: Stopped recording to database";
  //  }
}

bool BoardStationApp::isRecording() const
{
    return true;
    //return m_isRecording;
}


bool BoardStationApp::saveLiveData()
{
    qDebug() << "BoardStationApp: Starting to save live data to database";
    
    if (!m_parametersStorage || !m_boardMessagesWriter || !m_boardMessagesReader)
    {
        qWarning() << "BoardStationApp: Required components not available for saving live data";
        return false;
    }
    
    // Получаем все параметры из хранилища
    QList<BoardParameterSingle*> liveParameters = m_parametersStorage->getSessionParameters();
    
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
    
    // Добавляем новую сессию в модель списка сессий
    if (m_sessionsListModel)
    {
        m_sessionsListModel->addRecordedSession(sessionInfo);
        qDebug() << "BoardStationApp: Added new session to sessions list model";
    }
    
    // Очищаем хранилище параметров
    m_parametersStorage->clear();
    qDebug() << "BoardStationApp: Cleared parameters storage";
    
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
    
    // Сбрасываем счетчики живой сессии
    if (m_sessionsListModel)
    {
        m_sessionsListModel->resetLiveSessionCounters();
        qDebug() << "BoardStationApp: Reset live session counters";
    }
    
    qDebug() << "BoardStationApp: Successfully saved live data to session" << newSessionId;

    return true;
}

void BoardStationApp::loadSession(int sessionId)
{
    if (m_parametersStorage)
    {
        // Создаем новый SessionPlayer для этой сессии
        SessionPlayer* sessionPlayer = new SessionPlayer(this);
        sessionPlayer->setStorage(m_parametersStorage);
        sessionPlayer->setReader(m_boardMessagesReader);
        
        // Переключаемся на SessionPlayer
        m_dataPlayer = sessionPlayer;
        
        // Загружаем данные сессии
        m_parametersStorage->loadSessionData(sessionId, m_boardMessagesReader);
        
        qDebug() << "BoardStationApp: Loading session" << sessionId << "into new SessionPlayer";
    }
    else
    {
        qWarning() << "BoardStationApp: ParametersStorage is not available";
    }
}
