#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/Parameters/BoardParametersJsonParserNew.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/Parameters/BoardParameterHistoryStorage.h"
#include "Model/Parameters/OutParametersStorage.h"
#include "Model/Parameters/AppConfigurationReader.h"
#include "Model/Parameters/OutParametersParser.h"
#include "Model/Parameters/UplinkParametersParser.h"
#include "Model/Parameters/BoolUplinkParameter.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

BoardStationApp::BoardStationApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(nullptr)
    , m_parametersModel(nullptr)
    , m_driver(nullptr)
    , m_jsonReader(new BoardParametersJsonParserNew(this))
    , m_boardMessagesWriter(new BoardMessagesJsonWriterNew("CaptureBoardData.json", this))
{
    //qDebug() << "BoardStationApp: Application initialization";
    
    // Создаем хранилище параметров
    m_parametersStorage = new BoardParameterHistoryStorage(this);
    
    // Создаем модель параметров
    m_parametersModel = new BoardParametersListModel(m_parametersStorage, this);
    
    // Создаем хранилище исходящих параметров
    m_outParametersStorage = new OutParametersStorage(this);
    
    // Создаем модель исходящих параметров
    m_outParametersModel = new OutParametersModel(this);
    
    // Устанавливаем хранилище в модель
    m_outParametersModel->setStorage(m_outParametersStorage);
    
    // Создаем модель uplink параметров
    m_uplinkParametersModel = new UplinkParametersModel(this);
    
    // Подключаем сигнал изменения параметра к слоту
    connect(m_uplinkParametersModel, &UplinkParametersModel::parameterChanged,
            this, &BoardStationApp::onParameterChanged);
    
    // Создаем модель отладки
    m_debugViewModel = new DebugViewModel(this);
    
    // Создаем модель серий графиков
    m_chartSeriesModel = new ChartSeriesModel(this);
    m_chartSeriesModel->setParametersStorage(m_parametersStorage);
    
    // Загружаем исходящие параметры
    loadOutParameters();
    
    // Загружаем новые uplink параметры
    loadUplinkParameters();
    
    // Очищаем файл записи сообщений от борта
    m_boardMessagesWriter->clearFile();
    
    // Настраиваем драйвер
    setupDriver();
    
    // Подключаем сигналы
    connectSignals();
}

BoardStationApp::~BoardStationApp()
{
    //qDebug() << "BoardStationApp: Application shutdown";
}

void BoardStationApp::setMainWindow(MainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    
    if (m_mainWindow)
    {
        // Устанавливаем родителя для моделей - главное окно
        if (m_parametersModel) 
        {
            m_parametersModel->setParent(m_mainWindow);
        }
        if (m_outParametersModel) 
        {
            m_outParametersModel->setParent(m_mainWindow);
        }
        if (m_uplinkParametersModel) 
        {
            m_uplinkParametersModel->setParent(m_mainWindow);
        }
        if (m_debugViewModel) 
        {
            m_debugViewModel->setParent(m_mainWindow);
        }
    }
}

MainWindow* BoardStationApp::getMainWindow() const
{
    return m_mainWindow;
}

BoardParametersListModel* BoardStationApp::getParametersModel() const
{
    return m_parametersModel;
}

OutParametersModel* BoardStationApp::getOutParametersModel() const
{
    return m_outParametersModel;
}

UplinkParametersModel* BoardStationApp::getUplinkParametersModel() const
{
    return m_uplinkParametersModel;
}

DebugViewModel* BoardStationApp::getDebugViewModel() const
{
    return m_debugViewModel;
}

ChartSeriesModel* BoardStationApp::getChartSeriesModel() const
{
    return m_chartSeriesModel;
}

BoardParameterHistoryStorage* BoardStationApp::getParametersStorage() const
{
    return m_parametersStorage;
}

void BoardStationApp::setupDriver()
{
    // Создаем эмулятор данных платы и присваиваем его интерфейсу
    m_driver = new drv::BoardDataEmulator(this);
    
    //qDebug() << "BoardStationApp: Driver configured";
}

void BoardStationApp::connectSignals()
{
    if (m_driver)
    {
        connect(m_driver, &drv::IDriver::dataAvailable, this, &BoardStationApp::onDataAvailable);
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
    
    // Добавляем параметры в хранилище (создаёт или дополняет истории)
    m_parametersStorage->addParameters(newParameters);
    
    // Добавляем сообщение в очередь для записи в файл
    //m_boardMessagesWriter->addMessage(newParameters);
}

void BoardStationApp::loadOutParameters() const
{
    //qDebug() << "BoardStationApp: Loading out parameters from configuration.json";
    
    // Создаем читатель конфигурации
    AppConfigurationReader reader;
    
    // Формируем полный путь к файлу конфигурации
    QString configPath = QApplication::applicationDirPath() + "/configuration.json";
    //qDebug() << "BoardStationApp: Configuration path:" << configPath;
    
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
    
    // Создаем парсер параметров
    OutParametersParser parser;
    
    // Парсим параметры
    QList<OutParameter*> parsedParameters = parser.parseParameters(parametersArray);
    
    // Добавляем параметры в хранилище
    m_outParametersStorage->addParameters(parsedParameters);
    
    //qDebug() << "BoardStationApp: Successfully loaded" << parsedParameters.size() << "out parameters";
}

OutParametersStorage* BoardStationApp::getOutParametersStorage() const
{
    return m_outParametersStorage;
}

BoardMessagesJsonWriterNew* BoardStationApp::getBoardMessagesWriter() const
{
    return m_boardMessagesWriter;
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
