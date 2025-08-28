#ifndef CONFIGURATIONREADER_H
#define CONFIGURATIONREADER_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ConfigurationReader
{
public:
    ConfigurationReader();
    
    // Загружает конфигурацию из файла
    bool loadConfiguration(const QString &filePath);
    
    // Извлекает узел Parameters из конфигурации
    QJsonArray getParametersNode() const;
    
    // Получает всю конфигурацию
    QJsonObject getConfiguration() const { return m_configuration; }
    
    // Проверяет, загружена ли конфигурация
    bool isLoaded() const { return !m_configuration.isEmpty(); }

private:
    QJsonObject m_configuration;
};

#endif // CONFIGURATIONREADER_H
