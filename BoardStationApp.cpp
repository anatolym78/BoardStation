#include "BoardStationApp.h"
#include "mainwindow.h"
#include "Model/JsonReader.h"
#include "Model/Emulation/BoardDataEmulator.h"
#include "Model/ParametersStorage.h"
#include <QDebug>

BoardStationApp::BoardStationApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(nullptr)
    , m_parametersModel(nullptr)
    , m_enginesModel(nullptr)
    , m_driver(nullptr)
    , m_jsonReader(new JsonReader(this))
{
    qDebug() << "BoardStationApp: Инициализация приложения";
    
    // Создаем хранилище параметров
    m_parametersStorage = new ParametersStorage(this);
    
    // Создаем модель параметров
    m_parametersModel = new ParametersListModel(m_parametersStorage, this);
    
    // Создаем модель двигателей
    m_enginesModel = new EnginesModel(this);
    
    // Настраиваем драйвер
    setupDriver();
    
    // Подключаем сигналы
    connectSignals();
}

BoardStationApp::~BoardStationApp()
{
    qDebug() << "BoardStationApp: Завершение работы приложения";
}

void BoardStationApp::setMainWindow(MainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    
    if (m_mainWindow && m_parametersModel)
    {
        // Устанавливаем родителя для модели - главное окно
        m_parametersModel->setParent(m_mainWindow);
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

EnginesModel* BoardStationApp::getEnginesModel() const
{
    return m_enginesModel;
}

ParametersStorage* BoardStationApp::getParametersStorage() const
{
    return m_parametersStorage;
}

void BoardStationApp::setupDriver()
{
    // Создаем эмулятор данных платы и присваиваем его интерфейсу
    m_driver = new drv::BoardDataEmulator(this);
    
    qDebug() << "BoardStationApp: Драйвер настроен";
}

void BoardStationApp::connectSignals()
{
    if (m_driver)
    {
        connect(m_driver, &drv::IDriver::dataAvailable,
                this, &BoardStationApp::onDataAvailable);
        qDebug() << "BoardStationApp: Сигналы подключены";
    }
}

void BoardStationApp::onDataAvailable()
{
    qDebug() << "BoardStationApp: Получены новые данные от драйвера";
    
    if (!m_parametersModel || !m_driver) return;
    
    // Читаем данные от драйвера
    QString data = QString::fromStdString(m_driver->read());
    qDebug() << "BoardStationApp: Прочитаны данные от драйвера:" << data;
    
    if (data.isEmpty()) {
        qDebug() << "BoardStationApp: Данные от драйвера пусты";
        return;
    }
    
    // Парсим JSON и обновляем модель
    QList<Parameter> newParameters = m_jsonReader->parseParametersFromString(data);
    
    if (newParameters.isEmpty())
    {
        qDebug() << "BoardStationApp: Не удалось извлечь параметры из данных драйвера";

        return;
    }
    
    // Обновляем или добавляем параметры в хранилище
    m_parametersStorage->addParameters(newParameters);
}
