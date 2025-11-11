#ifndef BOARDMESSAGESSQLITEWRITER_H
#define BOARDMESSAGESSQLITEWRITER_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include "BoardParameterSingle.h"
#include "Model/Parameters/Tree/ParameterTreeStorage.h"

class BoardMessagesSqliteWriter : public QObject
{
	Q_OBJECT

public:
	explicit BoardMessagesSqliteWriter(const QString &databasePath = "BoardStationData.db", 
									  QObject *parent = nullptr);
	~BoardMessagesSqliteWriter();
	
	// Добавление сообщения в очередь
	void addMessage(const QList<BoardParameterSingle*> &parameters, 
				   const QDateTime &timestamp = QDateTime::currentDateTime());
	
	// Управление сессиями
	void createNewSession(const QString &sessionName = QString());
	void switchToSession(int sessionId);
	void switchToSession(const QString &sessionName);
	QStringList getAvailableSessions() const;
	int getCurrentSessionId() const { return m_currentSessionId; }
	QString getCurrentSessionName() const;
	
	// Принудительная запись всех сообщений из очереди
	void flushQueue();
	
	// Принудительное сохранение данных на диск
	void forceSave();
	
	// Установка интервала записи (в миллисекундах)
	void setWriteInterval(int intervalMs);
	int getWriteInterval() const { return m_writeInterval; }
	
	// Получение пути к файлу базы данных
	QString getDatabasePath() const { return m_databasePath; }
	
	// Очистка данных текущей сессии
	void clearCurrentSession();
	
	// Получение статистики
	int getMessageCount() const;
	int getParameterCount() const;

	// Сохранение древовидной модели параметров в текущую сессию
	// Проходит по всем узлам-историям и записывает их значения
	bool writeTree(ParameterTreeStorage* storage);

signals:
	void writeError(const QString &error);
	void writeSuccess(const QString &message);
	void sessionCreated(int sessionId, const QString &sessionName);
	void sessionSwitched(int sessionId, const QString &sessionName);
	void sessionCleared(int sessionId);

private slots:
	void writeMessagesToDatabase();

private:
	// Структура для хранения сообщения
	struct ParameterMessage 
	{
		QList<BoardParameterSingle*> parameters;
		QDateTime timestamp;
	};

	// Инициализация базы данных
	bool initializeDatabase();
	
	// Создание таблиц
	bool createTables();
	
	// Создание новой сессии
	int createSessionRecord(const QString &sessionName);
	
	// Запись сообщения в базу данных
	bool writeMessageToDatabase(const ParameterMessage *message);
	
	// Получение ID параметра по метке (создание при необходимости)
	int getParameterId(const QString &label, const QString &unit);

private:
	QString m_databasePath;
	QSqlDatabase m_database;
	QTimer *m_writeTimer;
	QQueue<ParameterMessage*> m_messageQueue;
	int m_writeInterval;
	int m_currentSessionId;
	QMutex m_queueMutex;
	mutable QMutex m_databaseMutex;
};

#endif // BOARDMESSAGESSQLITEWRITER_H
