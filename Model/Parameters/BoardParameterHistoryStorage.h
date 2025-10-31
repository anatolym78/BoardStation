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
	
	// Очистка всех параметров
	void clear();

	bool containsParameter(const QString& label);
	
	// Получение списка всех параметров сессии
	QList<BoardParameterSingle*> getSessionParameters() const;
	
	// Получение параметров в заданном временном диапазоне
	QList<BoardParameterSingle*> getParametersInTimeRange(const QDateTime &startTime, const QDateTime &endTime) const;
	QList<BoardParameterSingle*> getParametersInTimeRange(const QDateTime& startTime, const QDateTime& endTime, const QString& label) const;
	
	// Подсчет количества сообщений (параметры с одинаковой временной меткой считаются одним сообщением)
	int getMessagesCount() const;

signals:
	void newParameterAdded(BoardParameterSingle* parameter);
	void parameterAdded(const QString &label);
	void parameterUpdated(const QString &label);
	void parametersCleared();
	void sessionDataLoaded(int sessionId);
	
	// Новый сигнал для эмиссии параметров в плеер
	void parameterEmitted(BoardParameterSingle* parameter);

private:
	QList<BoardParameterSingle*> m_sessionParameters; // Список всех параметров сессии
};

#endif // BOARDPARAMETERHISTORYSTORAGE_H
