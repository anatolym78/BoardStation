#include "ViewModel/Parameters/Parameters.h"
#include <QDebug>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Создаем читатель конфигурации
    ConfigurationReader reader;
    
    // Загружаем конфигурацию
    if (!reader.loadConfiguration("config_example.json")) {
        qCritical() << "Не удалось загрузить конфигурацию";
        return -1;
    }
    
    // Получаем узел с параметрами
    QJsonArray parametersArray = reader.getParametersNode();
    if (parametersArray.isEmpty()) {
        qCritical() << "Узел Parameters пуст или не найден";
        return -1;
    }
    
    // Создаем парсер параметров
    ParametersParser parser;
    
    // Парсим параметры
    QList<OutParameter*> parameters = parser.parseParameters(parametersArray);
    
    qDebug() << "Создано параметров:" << parameters.size();
    
    // Выводим информацию о каждом параметре
    for (OutParameter *param : parameters) {
        qDebug() << "Параметр:" << param->getLabel();
        qDebug() << "  Тип контрола:" << param->getControlType();
        qDebug() << "  Значение:" << param->getValueAsString();
        qDebug() << "  Валиден:" << param->isValid();
        qDebug() << "---";
    }
    
    // Очищаем память
    parser.clearParameters(parameters);
    
    qDebug() << "Тест завершен успешно";
    
    return 0;
}
