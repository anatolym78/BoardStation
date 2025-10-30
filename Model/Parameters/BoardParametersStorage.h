#ifndef BOARDPARAMETERSSTORAGE_H
#define BOARDPARAMETERSSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include "BoardParameterSingle.h"

class BoardParametersStorage : public QObject
{
    Q_OBJECT

public:
    explicit BoardParametersStorage(QObject *parent = nullptr);
    
    // Добавление параметров - раскидывает по словарю по метке
    void addParameters(const QList<BoardParameterSingle*> &parameters);
    
    // Добавление одного параметра
    void addParameter(BoardParameterSingle *parameter);
    
    // Получение последнего значения параметра по метке
    BoardParameterSingle* lastValue(const QString &label) const;
    
    // Получение параметра по метке
    BoardParameterSingle* getParameter(const QString &label) const;
    
    // Получение всех меток параметров
    QStringList getParameterLabels() const;
    
    // Получение количества параметров
    int parameterCount() const;
    
    // Проверка существования параметра
    bool hasParameter(const QString &label) const;
    
    // Очистка всех параметров
    void clear();
    
    // Получение всех параметров как список
    QList<BoardParameterSingle*> getAllParameters() const;

signals:
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();

private:
    QMap<QString, BoardParameterSingle*> m_parameters;
};

#endif // BOARDPARAMETERSSTORAGE_H
