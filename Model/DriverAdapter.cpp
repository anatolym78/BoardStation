#include "DriverAdapter.h"
#include "Model/Parameters/BoardParametersJsonParserNew.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

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
    if (!m_driver)
    {
        qWarning() << "DriverAdapter: Driver is not available";
        return;
    }
    
    // Читаем данные от драйвера
    QString data = QString::fromStdString(m_driver->read());
    
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

void DriverAdapter::sendParameter(BasicUplinkParameter* parameter)
{
    if (!parameter || !parameter->isValid())
    {
        qWarning() << "DriverAdapter: Invalid parameter for sending";
        return;
    }
    
    if (!m_driver)
    {
        qWarning() << "DriverAdapter: Driver is not available for parameter sending";
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
    
    qDebug() << "DriverAdapter: Parameter sent:" << parameter->getLabel() << "=" << parameter->getValue();
}
