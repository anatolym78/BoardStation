#ifndef CHATVIEWGRIDMODEL_H
#define CHATVIEWGRIDMODEL_H

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

class ChatViewGridModel : public QAbstractListModel
{
	Q_OBJECT
		
	Q_PROPERTY(bool isCanMergeCharts READ isCanMergeCharts NOTIFY isCanMergeChartsChanged)

public:
	enum ChartViewRoles
	{
		LabelsRole = Qt::UserRole + 1,
		LabelRole,
		ChartIndexRole,
		DepthRole,
		SelectionRole,
		HoverRole,
	};

	explicit ChatViewGridModel(QObject *parent = nullptr);

	void setPlayer(DataPlayer* dataPlayer);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

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
	Q_INVOKABLE bool isEmpty() const { return m_series.isEmpty(); }

	Q_INVOKABLE bool selectElement(int index, bool keepSelection);
	Q_INVOKABLE void clearSelection();

	Q_INVOKABLE bool hoverElement(int index);
	Q_INVOKABLE void clearHover();

	Q_INVOKABLE bool isCanMergeCharts() const;

	void updateAllCells();
	
signals:
	void chartDataAdded(const QString &chartLabel, const QString &parameterLabel);
	void parameterAdded(const QString& label, const QColor& color);
	void isCanMergeChartsChanged();

	struct ChartInfo
	{
		QStringList series;
		QColor color = Qt::darkGray;
		int depth = 0;
		bool isSelected = false;
	};

private:
	DataPlayer* m_dataPlayer = nullptr;
	//QMetaObject::Connection m_playConnection;
	//QMetaObject::Connection m_stopConnection;
	QList<ChartInfo> m_charts;
	QList<QStringList> m_series;
	QList<int> m_depths;
	QList<bool> m_selectedIndices;
	int m_hoverIndex = -1;

	// Вспомогательные методы
	int findChartIndex(const QString &label) const;
	bool parameterExistsInHistory(const QString &label) const;
};

#endif // CHATVIEWGRIDMODEL_H
