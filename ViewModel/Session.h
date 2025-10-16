#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QDateTime>
#include <QString>

#include "./DataPlayer.h"
#include "./BoardParametersListModel.h"
#include "./ChartViewModel.h"

class BoardParameterHistoryStorage;

/**
 * @brief Базовый класс для представления сессии
 */
class Session : public QObject
{
	Q_OBJECT

public:
	enum SessionType
	{
		RecordedSession,
		LiveSession
	};

	explicit Session(QObject *parent = nullptr);
	virtual ~Session() = default;

	// Виртуальные методы для получения информации о сессии
	virtual int getId() const = 0;
	virtual QString getName() const = 0;
	virtual QDateTime getCreatedAt() const = 0;
	virtual QString getDescription() const = 0;
	virtual int getMessageCount() const = 0;
	virtual int getParameterCount() const = 0;
	virtual SessionType getType() const = 0;
	virtual bool isRecording() const = 0;

	// Методы для работы с хранилищем
	virtual BoardParameterHistoryStorage* getStorage() const = 0;
	virtual void clearStorage() = 0;

	// Методы для сравнения сессий (для сортировки)
	virtual bool operator<(const Session& other) const;
	virtual bool operator==(const Session& other) const;

	DataPlayer* player() const { return m_player; }
	BoardParametersListModel* parametersModel()  const;
	ChartViewModel* chartsModel() const { return m_chartsModel; }

	virtual void open() { m_opened = true; }

	bool isOpened() const { return m_opened; }

signals:
	void sessionChanged();
	void messageCountChanged(int count);
	void parameterCountChanged(int count);

protected:
	// Защищенные методы для обновления данных
	void updateMessageCount(int count);
	void updateParameterCount(int count);

protected:
	BoardParametersListModel* m_parametersModel;
	ChartViewModel* m_chartsModel;
	DataPlayer* m_player;
	bool m_opened;

};

#endif // SESSION_H
