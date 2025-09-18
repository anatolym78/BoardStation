#ifndef BOARDPARAMETERHISTORYSTORAGE_H
#define BOARDPARAMETERHISTORYSTORAGE_H

#include <QObject>
#include <QList>
#include <QString>
#include "BoardParameterHistory.h"
#include "BoardParameterSingle.h"

class BoardParameterHistoryStorage : public QObject
{
    Q_OBJECT

public:
    explicit BoardParameterHistoryStorage(QObject *parent = nullptr);
    
    // Добавление отдельных параметров - создаёт или дополняет истории
    void addParameters(const QList<BoardParameterSingle*> &parameters);
    
    // Добавление одного параметра
    void addParameter(BoardParameterSingle *parameter);
    
    // Получение истории параметра по метке
    BoardParameterHistory* getParameterHistory(const QString &label) const;
    
    // Получение всех меток параметров
    QStringList getParameterLabels() const;
    
    // Получение количества историй параметров
    int parameterHistoryCount() const;
    
    // Проверка существования истории параметра
    bool hasParameterHistory(const QString &label) const;
    
    // Очистка всех историй параметров
    void clear();

    int indexOf(const QString& label) const;
    
    // Получение всех историй параметров как список
    QList<BoardParameterHistory*> getAllParameterHistories() const;

signals:
    void newParameterAdded(BoardParameterSingle* parameter);
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();

private:
    // Поиск истории по метке
    BoardParameterHistory* findHistoryByLabel(const QString &label) const;

private:
    QList<BoardParameterHistory*> m_parameterHistories;
};

#endif // BOARDPARAMETERHISTORYSTORAGE_H
