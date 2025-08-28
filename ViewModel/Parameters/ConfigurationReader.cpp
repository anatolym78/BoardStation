#include "ConfigurationReader.h"
#include <QFile>
#include <QJsonParseError>
#include <QDebug>

ConfigurationReader::ConfigurationReader()
{
}

bool ConfigurationReader::loadConfiguration(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ConfigurationReader: Не удалось открыть файл:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "ConfigurationReader: Ошибка парсинга JSON:" << error.errorString();
        return false;
    }
    
    if (!doc.isObject()) {
        qWarning() << "ConfigurationReader: JSON должен содержать объект";
        return false;
    }
    
    m_configuration = doc.object();
    qDebug() << "ConfigurationReader: Конфигурация успешно загружена из" << filePath;
    
    return true;
}

QJsonArray ConfigurationReader::getParametersNode() const
{
    if (m_configuration.contains("Parameters")) {
        QJsonValue paramsValue = m_configuration["Parameters"];
        if (paramsValue.isArray()) {
            return paramsValue.toArray();
        }
    }
    
    qWarning() << "ConfigurationReader: Узел Parameters не найден или не является массивом";
    return QJsonArray();
}
