#ifndef UPLINKPARAMETERSPARSER_H
#define UPLINKPARAMETERSPARSER_H

#include "BasicUplinkParameter.h"
#include <QJsonArray>
#include <QList>

class UplinkParametersParser
{
public:
    UplinkParametersParser();
    
    // Парсит массив параметров из JSON
    QList<BasicUplinkParameter*> parseParameters(const QJsonArray &parametersArray);
    
    // Парсит один параметр
    BasicUplinkParameter* parseParameter(const QJsonObject &parameterObj);
    
    // Очищает список параметров
    void clearParameters(QList<BasicUplinkParameter*> &parameters);

private:
    // Создает параметр на основе типа
    BasicUplinkParameter* createParameter(const QString &label, 
                                         const QString &valueType,
                                         const QString &controlType,
                                         const QJsonObject &parameterObj);
    
    // Корректирует тип контрола согласно таблице соответствия
    QString correctControlType(const QString &valueType, const QString &controlType, const QJsonObject &parameterObj);
    
    // Получает значение по умолчанию для типа параметра
    QJsonValue getDefaultValueForType(const QString &valueType);
    
    // Проверяет является ли тип контрола подходящим для параметра
    bool isControlTypeSuitable(const QString &controlType, const QString &valueType, const QJsonObject &parameterObj);
    
    // Отдельные функции для проверки каждого типа контрола
    bool isTextEditSuitable(const QString &valueType, const QJsonObject &parameterObj);
    bool isSliderSuitable(const QString &valueType, const QJsonObject &parameterObj);
    bool isCheckBoxSuitable(const QString &valueType, const QJsonObject &parameterObj);
    bool isSwitchSuitable(const QString &valueType, const QJsonObject &parameterObj);
    bool isSpinBoxSuitable(const QString &valueType, const QJsonObject &parameterObj);
    bool isComboBoxSuitable(const QString &valueType, const QJsonObject &parameterObj);
    
    // Рассчитывает количество возможных значений для диапазона и шага
    int calculatePossibleValuesCount(double min, double max, double step);
    
    // Парсит диапазон для числового параметра
    bool parseRange(const QJsonObject &parameterObj, 
                   double &minimum, double &maximum, double &step);
    
    // Парсит список значений для числового параметра
    QList<double> parseValuesList(const QJsonObject &parameterObj);
    
    // Парсит список строковых значений
    QStringList parseStringValuesList(const QJsonObject &parameterObj);
    
    // Парсит список булевых значений
    QList<bool> parseBoolValuesList(const QJsonObject &parameterObj);
};

#endif // UPLINKPARAMETERSPARSER_H
