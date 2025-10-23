#ifndef BOARDPARAMETERSLISTMODEL_H
#define BOARDPARAMETERSLISTMODEL_H

#include <QAbstractListModel>
#include <QColor>

#include "Model/Parameters/BoardMessagesSqliteReader.h"

#include "./DataPlayer.h"

class BoardParametersListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum class ParameterRole: int
	{
		LabelRole = Qt::UserRole + 1,
		ValueRole,
		UntiRole,
		TimeRole,
		ChartVisibilityRole,
		ColorRole,
	};
public:
	explicit BoardParametersListModel(QObject* parent = nullptr);

	void setPlayer(DataPlayer* dataPlayer);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE int getCountParameters() const;
	
	// Публичный метод для очистки параметров
	Q_INVOKABLE void clearParameters();

public slots :
	void onNewParameterAdded(BoardParameterSingle* param);
	void onParametersCleared();

private:
	QMap<QString, BoardParameterSingle*> m_values;
	QList<bool> m_chartVisibilities;
	QList<QColor> m_colors;

	DataPlayer* m_dataPlayer = nullptr;
	QMetaObject::Connection m_playConnection;
	QMetaObject::Connection m_stopConnection;

private:
	void makeRandomColors();
};

#endif // BOARDPARAMETERSLISTMODEL_H
