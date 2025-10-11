#include "DriverAdapter.h"
#include "Model/Parameters/BoardParametersJsonParserNew.h"
#include <QDebug>

DriverAdapter::DriverAdapter(drv::IDriver* driver, QObject *parent)
    : QObject(parent)
    , m_driver(driver)
    , m_jsonParser(new BoardParametersJsonParserNew(this))
    , m_isConnected(false)
{
    if (m_driver)
    {
        connectToDriver();
        // Автоматически запускаем прослушивание
        startListening();
    }
}

DriverAdapter::~DriverAdapter()
{
    disconnectFromDriver();
}

void DriverAdapter::setDriver(drv::IDriver* driver)
{
    if (m_driver == driver)
    {
        return;
    }

    disconnectFromDriver();
    m_driver = driver;
    
    if (m_driver)
    {
        connectToDriver();
        // Автоматически запускаем прослушивание
        startListening();
    }
}

drv::IDriver* DriverAdapter::getDriver() const
{
    return m_driver;
}

void DriverAdapter::startListening()
{
    if (m_driver)
    {
        m_driver->startListening();
        qDebug() << "DriverAdapter: Started listening";
    }
}

void DriverAdapter::stopListening()
{
    if (m_driver)
    {
        m_driver->stopListening();
        qDebug() << "DriverAdapter: Stopped listening";
    }
}

bool DriverAdapter::isListening() const
{
    if (m_driver)
    {
        return m_driver->isListening();
    }
    return false;
}

void DriverAdapter::onDriverDataAvailable()
{
    qDebug() << "DriverAdapter: Received data from driver";
    
    if (!m_driver)
    {
        qWarning() << "DriverAdapter: Driver is not available";
        return;
    }
    
    // Читаем данные от драйвера
    QString data = QString::fromStdString(m_driver->read());
    qDebug() << "DriverAdapter: Read data from driver:" << data;
    
    if (data.isEmpty())
    {
        qDebug() << "DriverAdapter: Driver data is empty";
        return;
    }
    
    // Парсим JSON и получаем отдельные параметры
    QList<BoardParameterSingle*> newParameters = m_jsonParser->parseParametersFromString(data);
    
    if (newParameters.isEmpty())
    {
        qDebug() << "DriverAdapter: Failed to extract parameters from driver data";
        return;
    }

    qDebug() << "DriverAdapter: Parsed" << newParameters.size() << "parameters";

    // Эмитируем сигнал для каждого параметра отдельно
    for (BoardParameterSingle* parameter : newParameters)
    {
        if (parameter)
        {
            emit parameterReceived(parameter);
        }
    }

    // Также эмитируем сигнал для всей группы параметров
    emit parametersReceived(newParameters);
}

void DriverAdapter::connectToDriver()
{
    if (m_driver && !m_isConnected)
    {
        connect(m_driver, &drv::IDriver::dataAvailable, 
                this, &DriverAdapter::onDriverDataAvailable);
        m_isConnected = true;
        qDebug() << "DriverAdapter: Connected to driver";
    }
}

void DriverAdapter::disconnectFromDriver()
{
    if (m_driver && m_isConnected)
    {
        disconnect(m_driver, &drv::IDriver::dataAvailable, 
                   this, &DriverAdapter::onDriverDataAvailable);
        m_isConnected = false;
        qDebug() << "DriverAdapter: Disconnected from driver";
    }
}
