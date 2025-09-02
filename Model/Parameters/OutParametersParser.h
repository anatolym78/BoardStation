#ifndef OUTPARAMETERSPARSER_H
#define OUTPARAMETERSPARSER_H

#include "OutParameter.h"
#include <QJsonArray>
#include <QList>

class OutParametersParser
{
public:
    OutParametersParser();
    
    // Парсит массив параметров из JSON
    QList<OutParameter*> parseParameters(const QJsonArray &parametersArray);
    
    // Парсит один параметр
    OutParameter* parseParameter(const QJsonObject &parameterObj);
    
    // Очищает список параметров
    void clearParameters(QList<OutParameter*> &parameters);

private:
    // Создает параметр на основе типа
    OutParameter* createParameter(const QString &label, 
                                 const QString &valueType,
                                 const QString &controlType,
                                 const QJsonObject &parameterObj);
    
    // Парсит диапазон для вещественного параметра
    bool parseRange(const QJsonObject &parameterObj, 
                   double &minimum, double &maximum, double &step);
    
    // Парсит список значений для вещественного параметра
    QList<double> parseValuesList(const QJsonObject &parameterObj);
    
    // Парсит список строковых значений
    QStringList parseStringValuesList(const QJsonObject &parameterObj);
};

#endif // OUTPARAMETERSPARSER_H
