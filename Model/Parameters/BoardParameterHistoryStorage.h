#ifndef BOARDPARAMETERHISTORYSTORAGE_H
#define BOARDPARAMETERHISTORYSTORAGE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include "BoardParameterHistory.h"
#include "BoardParameterSingle.h"

class BoardMessagesSqliteReader;

class BoardParameterHistoryStorage : public QObject
{
    Q_OBJECT

public:
    explicit BoardParameterHistoryStorage(QObject *parent = nullptr);
    
    // Добавление отдельных параметров - создаёт или дополняет истории
    void addParameters(const QList<BoardParameterSingle*> &parameters);
    
    // Добавление одного параметра
    void addParameter(BoardParameterSingle *parameter);
    
    // Загрузка данных сессии из базы данных
    void loadSessionData(int sessionId, BoardMessagesSqliteReader* reader);
    
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
    
    // Получение списка всех параметров сессии
    QList<BoardParameterSingle*> getSessionParameters() const;
    
    // Получение параметров в заданном временном диапазоне
    QList<BoardParameterSingle*> getParametersInTimeRange(const QDateTime &startTime, const QDateTime &endTime) const;

signals:
    void newParameterAdded(BoardParameterSingle* parameter);
    void parameterAdded(const QString &label);
    void parameterUpdated(const QString &label);
    void parametersCleared();
    void sessionDataLoaded(int sessionId);

private:
    // Поиск истории по метке
    BoardParameterHistory* findHistoryByLabel(const QString &label) const;

private:
    QList<BoardParameterHistory*> m_parameterHistories;
    QList<BoardParameterSingle*> m_sessionParameters; // Список всех параметров сессии
};

#endif // BOARDPARAMETERHISTORYSTORAGE_H
