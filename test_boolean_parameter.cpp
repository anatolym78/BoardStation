#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "Model/Parameters/BooleanOutParameter.h"
#include "Model/Parameters/OutParametersStorage.h"
#include "Model/Parameters/OutParametersParser.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "Тестирование BooleanParameter и OutParametersStorage";
    
    // Тест 1: Создание BooleanParameter
    qDebug() << "\n=== Тест 1: Создание BooleanParameter ===";
    
    BooleanOutParameter *cameraParam = new BooleanOutParameter(
        "Камера",           // label
        false,              // начальное значение
        "Выключена",        // алиас для false
        "Включена",         // алиас для true
        "QCheckBox"         // тип контрола
    );
    
    qDebug() << "Создан параметр:" << cameraParam->getLabel();
    qDebug() << "Текущее значение:" << cameraParam->getValue();
    qDebug() << "Алиас для false:" << cameraParam->getFalseAlias();
    qDebug() << "Алиас для true:" << cameraParam->getTrueAlias();
    qDebug() << "Текущий алиас:" << cameraParam->getValueAlias();
    qDebug() << "Тип контрола:" << cameraParam->getControlType();
    qDebug() << "Валидность:" << cameraParam->isValid();
    
    // Тест 2: Изменение значения
    qDebug() << "\n=== Тест 2: Изменение значения ===";
    
    cameraParam->setValue(true);
    qDebug() << "Новое значение:" << cameraParam->getValue();
    qDebug() << "Новый алиас:" << cameraParam->getValueAlias();
    
    // Тест 3: Работа с хранилищем
    qDebug() << "\n=== Тест 3: Работа с хранилищем ===";
    
    OutParametersStorage storage;
    storage.addParameter(cameraParam);
    
    qDebug() << "Количество параметров в хранилище:" << storage.parameterCount();
    qDebug() << "Метки параметров:" << storage.getParameterLabels();
    
    // Получение параметра по метке
    OutParameter *retrievedParam = storage.getParameter("Камера");
    if (retrievedParam) {
        qDebug() << "Получен параметр:" << retrievedParam->getLabel();
    }
    
    // Получение параметров определенного типа
    QList<BooleanOutParameter*> boolParams = storage.getParametersOfType<BooleanOutParameter>();
    qDebug() << "Количество BooleanOutParameter:" << boolParams.size();
    
    // Тест 4: Парсинг JSON
    qDebug() << "\n=== Тест 4: Парсинг JSON ===";
    
    // Создаем тестовый JSON
    QJsonObject paramObj;
    paramObj["label"] = "Двигатель";
    paramObj["valueType"] = "bool";
    paramObj["controlType"] = "QComboBox";
    
    QJsonObject aliasesObj;
    aliasesObj["false"] = "Остановлен";
    aliasesObj["true"] = "Работает";
    paramObj["aliases"] = aliasesObj;
    
    QJsonArray paramsArray;
    paramsArray.append(paramObj);
    
    // Парсим параметры
    OutParametersParser parser;
    QList<OutParameter*> parsedParams = parser.parseParameters(paramsArray);
    
    qDebug() << "Распарсено параметров:" << parsedParams.size();
    
    if (!parsedParams.isEmpty()) {
        OutParameter *parsedParam = parsedParams.first();
        qDebug() << "Распарсен параметр:" << parsedParam->getLabel();
        qDebug() << "Тип контрола:" << parsedParam->getControlType();
        qDebug() << "Валидность:" << parsedParam->isValid();
        
        // Добавляем в хранилище
        storage.addParameter(parsedParam);
        qDebug() << "Новое количество параметров:" << storage.parameterCount();
    }
    
    // Тест 5: Очистка
    qDebug() << "\n=== Тест 5: Очистка ===";
    
    storage.clear();
    qDebug() << "Количество параметров после очистки:" << storage.parameterCount();
    
    qDebug() << "\nТестирование завершено!";
    
    return 0;
}
