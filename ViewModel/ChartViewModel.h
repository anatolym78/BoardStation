#ifndef CHARTVIEWMODEL_H
#define CHARTVIEWMODEL_H

#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <QVariant>
#include <QColor>

#include "./DataPlayer.h"

class ChartViewModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum ChartViewRoles
	{
		ChartRole = Qt::UserRole + 1,
		ChartLabelRole,
		LabelsRole,
		LabelRole,
		ChartIndexRole,
		HasDataRole,
		DepthRole,
		IsExists,
	};

	explicit ChartViewModel(QObject *parent = nullptr);

	void setPlayer(DataPlayer* dataPlayer);

	// QAbstractListModel interface
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent /* = QModelIndex() */) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	int cellToIndex(int row, int col) const;
	int cellToIndex(const QModelIndex& modelIndex) const;
	QModelIndex indexToCell(int index) const;


	// Методы для работы с графиками
	Q_INVOKABLE bool toggleParameter(const QString& label, const QColor& color = Qt::red);
	Q_INVOKABLE void addChart(const QString &label, const QColor& color = Qt::red);
	Q_INVOKABLE void removeChart(int index);
	Q_INVOKABLE void removeLabel(const QString& label);
	Q_INVOKABLE void clearCharts();
	Q_INVOKABLE QStringList getChartSeriesLabels(int chartIndex) const;

	Q_INVOKABLE void reorderChartsBeforeDrag(int dragIndex);
	Q_INVOKABLE void resetDepths();

	Q_INVOKABLE void mergeCharts(int movedIndex, int targetIndex);
	Q_INVOKABLE void splitSeries(int chartIndex);
	
	Q_INVOKABLE QStringList chartLabels() const;

	Q_INVOKABLE bool hasSeries(const QString &label) const;
	Q_INVOKABLE int countSeries() const { return m_series.size(); }
	
signals:
	void chartDataAdded(const QString &chartLabel, const QString &parameterLabel);
	void parameterAdded(const QString& label, const QColor& color);

private:
	DataPlayer* m_dataPlayer = nullptr;
	QMetaObject::Connection m_playConnection;
	QMetaObject::Connection m_stopConnection;
	QList<QStringList> m_series;
	QList<int> m_depths;
	int m_columnCount = 2;

	// Вспомогательные методы
	int findChartIndex(const QString &label) const;
	bool parameterExistsInHistory(const QString &label) const;
};

#endif // CHARTVIEWMODEL_H
