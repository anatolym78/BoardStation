#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/Parameters/BoardParametersJsonParser.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/Parameters/BoardParametersStorage.h"
#include "Model/Parameters/OutParametersStorage.h"
#include "Model/Parameters/AppConfigurationReader.h"
#include "Model/Parameters/OutParametersParser.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

BoardStationApp::BoardStationApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(nullptr)
    , m_parametersModel(nullptr)
    , m_driver(nullptr)
    , m_jsonReader(new BoardParametersJsonParser(this))
    , m_boardMessagesWriter(new BoardMessagesJsonWriter("CaptureBoardData.json", this))
{
    qDebug() << "BoardStationApp: Application initialization";
    
    // Создаем хранилище параметров
    m_parametersStorage = new BoardParametersStorage(this);
    
    // Создаем модель параметров
    m_parametersModel = new BoardParametersModel(m_parametersStorage, this);
    
    // Создаем хранилище исходящих параметров
    m_outParametersStorage = new OutParametersStorage(this);
    
    // Создаем модель исходящих параметров
    m_outParametersModel = new OutParametersModel(this);
    
    // Устанавливаем хранилище в модель
    m_outParametersModel->setStorage(m_outParametersStorage);
    
    // Загружаем исходящие параметры
    loadOutParameters();
    
    // Очищаем файл записи сообщений от борта
    m_boardMessagesWriter->clearFile();
    
    // Настраиваем драйвер
    setupDriver();
    
    // Подключаем сигналы
    connectSignals();
}

BoardStationApp::~BoardStationApp()
{
    qDebug() << "BoardStationApp: Application shutdown";
}

void BoardStationApp::setMainWindow(MainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    
    if (m_mainWindow)
    {
        // Устанавливаем родителя для моделей - главное окно
        if (m_parametersModel) {
            m_parametersModel->setParent(m_mainWindow);
        }
        if (m_outParametersModel) {
            m_outParametersModel->setParent(m_mainWindow);
        }
    }
}

MainWindow* BoardStationApp::getMainWindow() const
{
    return m_mainWindow;
}

BoardParametersModel* BoardStationApp::getParametersModel() const
{
    return m_parametersModel;
}

OutParametersModel* BoardStationApp::getOutParametersModel() const
{
    return m_outParametersModel;
}

BoardParametersStorage* BoardStationApp::getParametersStorage() const
{
    return m_parametersStorage;
}

void BoardStationApp::setupDriver()
{
    // Создаем эмулятор данных платы и присваиваем его интерфейсу
    m_driver = new drv::BoardDataEmulator(this);
    
    qDebug() << "BoardStationApp: Driver configured";
}

void BoardStationApp::connectSignals()
{
    if (m_driver)
    {
        connect(m_driver, &drv::IDriver::dataAvailable,
                this, &BoardStationApp::onDataAvailable);
        qDebug() << "BoardStationApp: Signals connected";
    }
}

void BoardStationApp::onDataAvailable()
{
    qDebug() << "BoardStationApp: Received new data from driver";
    
    if (!m_parametersModel || !m_driver)
    {
        return;
    }
    
    // Читаем данные от драйвера
    QString data = QString::fromStdString(m_driver->read());
    qDebug() << "BoardStationApp: Read data from driver:" << data;
    
    if (data.isEmpty())
    {
        qDebug() << "BoardStationApp: Driver data is empty";
        return;
    }
    
    // Парсим JSON и обновляем модель
    QList<BoardParameter> newParameters = m_jsonReader->parseParametersFromString(data);
    
    if (newParameters.isEmpty())
    {
        qDebug() << "BoardStationApp: Failed to extract parameters from driver data";

        return;
    }
    
    // Обновляем или добавляем параметры в хранилище
    m_parametersStorage->addParameters(newParameters);
    
    // Добавляем сообщение в очередь для записи в файл
    m_boardMessagesWriter->addMessage(newParameters);
}

void BoardStationApp::loadOutParameters()
{
    qDebug() << "BoardStationApp: Loading out parameters from configuration.json";
    
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
    
    // Создаем парсер параметров
    OutParametersParser parser;
    
    // Парсим параметры
    QList<OutParameter*> parsedParameters = parser.parseParameters(parametersArray);
    
    // Добавляем параметры в хранилище
    m_outParametersStorage->addParameters(parsedParameters);
    
    qDebug() << "BoardStationApp: Successfully loaded" << parsedParameters.size() << "out parameters";
}

OutParametersStorage* BoardStationApp::getOutParametersStorage() const
{
    return m_outParametersStorage;
}

BoardMessagesJsonWriter* BoardStationApp::getBoardMessagesWriter() const
{
    return m_boardMessagesWriter;
}

void BoardStationApp::sendParametersToBoard()
{
    qDebug() << "BoardStationApp: Sending parameters to board";
    
    if (!m_outParametersStorage) {
        qWarning() << "BoardStationApp: OutParametersStorage is not available";
        return;
    }
    
    // Получаем все исходящие параметры
    QList<OutParameter*> outParameters = m_outParametersStorage->getAllParameters();
    
    if (outParameters.isEmpty()) {
        qDebug() << "BoardStationApp: No out parameters to send";
        return;
    }
    
    // Создаем JSON массив из параметров
    QJsonArray parametersArray;
    for (OutParameter *param : outParameters) {
        if (param && param->isValid()) {
            QJsonObject paramObj = param->toJsonObject();
            parametersArray.append(paramObj);
        }
    }
    
    if (parametersArray.isEmpty()) {
        qDebug() << "BoardStationApp: No valid parameters to send";
        return;
    }
    
    // Создаем JSON документ
    QJsonDocument doc(parametersArray);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    
    qDebug() << "BoardStationApp: Sending JSON to board:" << jsonString;
    
    // Отправляем данные через драйвер
    if (m_driver) {
        std::string data = jsonString.toStdString();
        m_driver->write(data);
        qDebug() << "BoardStationApp: Parameters sent to board successfully";
    } else {
        qWarning() << "BoardStationApp: Driver is not available";
    }
}
