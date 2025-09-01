#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/JsonReader.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/ParametersStorage.h"
#include "ViewModel/Parameters/OutParametersStorage.h"
#include "ViewModel/Parameters/ConfigurationReader.h"
#include "ViewModel/Parameters/ParametersParser.h"
#include <QDebug>

BoardStationApp::BoardStationApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(nullptr)
    , m_parametersModel(nullptr)
    , m_driver(nullptr)
    , m_jsonReader(new JsonReader(this))
{
    qDebug() << "BoardStationApp: Application initialization";
    
    // Создаем хранилище параметров
    m_parametersStorage = new ParametersStorage(this);
    
    // Создаем модель параметров
    m_parametersModel = new ParametersListModel(m_parametersStorage, this);
    
    // Создаем хранилище исходящих параметров
    m_outParametersStorage = new OutParametersStorage(this);
    
    // Создаем модель исходящих параметров
    m_outParametersModel = new OutParametersModel(this);
    
    // Устанавливаем хранилище в модель
    m_outParametersModel->setStorage(m_outParametersStorage);
    
    // Загружаем исходящие параметры
    loadOutParameters();
    
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

ParametersListModel* BoardStationApp::getParametersModel() const
{
    return m_parametersModel;
}

OutParametersModel* BoardStationApp::getOutParametersModel() const
{
    return m_outParametersModel;
}

ParametersStorage* BoardStationApp::getParametersStorage() const
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
    QList<Parameter> newParameters = m_jsonReader->parseParametersFromString(data);
    
    if (newParameters.isEmpty())
    {
        qDebug() << "BoardStationApp: Failed to extract parameters from driver data";

        return;
    }
    
    // Обновляем или добавляем параметры в хранилище
    m_parametersStorage->addParameters(newParameters);
}

void BoardStationApp::loadOutParameters()
{
    qDebug() << "BoardStationApp: Loading out parameters from configuration.json";
    
    // Создаем читатель конфигурации
    ConfigurationReader reader;
    
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
    ParametersParser parser;
    
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
