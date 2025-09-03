#ifndef BOARDPARAMETERSSTORAGE_H
#define BOARDPARAMETERSSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include "BoardParameter.h"

class BoardParametersStorage : public QObject
{
    Q_OBJECT

public:
    explicit BoardParametersStorage(QObject *parent = nullptr);
    
    // Добавление параметров - раскидывает по словарю по метке
    void addParameters(const QList<BoardParameter*> &parameters);
    
    // Добавление одного параметра
    void addParameter(BoardParameter *parameter);
    
    // Получение последнего значения параметра по метке
    BoardParameter* lastValue(const QString &label) const;
    
    // Получение параметра по метке
    BoardParameter* getParameter(const QString &label) const;
    
    // Получение всех меток параметров
    QStringList getParameterLabels() const;
    
    // Получение количества параметров
    int parameterCount() const;
    
    // Проверка существования параметра
    bool hasParameter(const QString &label) const;
    
    // Очистка всех параметров
    void clear();
    
    // Получение всех параметров как список
    QList<BoardParameter*> getAllParameters() const;

signals:
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();

private:
    QMap<QString, BoardParameter*> m_parameters;
};

#endif // BOARDPARAMETERSSTORAGE_H
