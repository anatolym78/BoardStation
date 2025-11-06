#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QDateTime>
#include <QString>

#include "./DataPlayer.h"
#include "./BoardParametersListModel.h"
#include "./BoardParametersTreeModel.h"
//#include "./ChartViewModel.h"
#include "./ChatViewGridModel.h"

class BoardParameterHistoryStorage;
class ParameterTreeStorage;

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
	ParameterTreeStorage* storage() const;
	virtual void clearStorage() = 0;

	// Методы для сравнения сессий (для сортировки)
	virtual bool operator<(const Session& other) const;
	virtual bool operator==(const Session& other) const;

	DataPlayer* player() const { return m_player; }
	BoardParametersTreeModel* parametersModel()  const;
	ChatViewGridModel* chartsModel() const { return m_chartsModel; }

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
	BoardParametersTreeModel* m_parametersModel;
	ChatViewGridModel* m_chartsModel;
	DataPlayer* m_player;
	ParameterTreeStorage* m_treeStorage;
	bool m_opened;

};

#endif // SESSION_H
