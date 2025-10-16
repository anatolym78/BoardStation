#ifndef CHARTVIEWMODEL_H
#define CHARTVIEWMODEL_H

#include <QAbstractListModel>
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

class ChartViewModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum ChartViewRoles
	{
		ChartRole = Qt::UserRole + 1,
		ChartLabelRole,
		ChartIndexRole,
		HasDataRole,
		DepthRole,
	};

	explicit ChartViewModel(QObject *parent = nullptr);

	void setPlayer(DataPlayer* dataPlayer);

	// QAbstractListModel interface
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
	
	// Методы для работы с данными
	Q_INVOKABLE void addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
								  double x, double y, const QDateTime &timestamp, const QVariant &value);
	Q_INVOKABLE void addDataPoint(const QString &chartLabel, const QString &parameterLabel, 
								  double x, double y);
	
	// Геттеры
	Q_INVOKABLE QtCharts::QChart* getChart(int index) const;
	Q_INVOKABLE QtCharts::QChart* getChart(const QString &label) const;
	Q_INVOKABLE QStringList chartLabels() const;
	
	// Методы для получения данных серий
	Q_INVOKABLE QVariantList getSeriesData(const QString &chartLabel, const QString &parameterLabel) const;
	Q_INVOKABLE QStringList getParameterLabels(const QString &chartLabel) const;
	Q_INVOKABLE int getSeriesPointCount(const QString &chartLabel, const QString &parameterLabel) const;
	
	// Проверки
	Q_INVOKABLE bool hasSeries(const QString &label) const;
	Q_INVOKABLE int countSeries() const { return m_series.size(); }

	void onParameterPlayed(BoardParameterSingle* parameter);
	
signals:
	void chartDataAdded(const QString &chartLabel, const QString &parameterLabel);
	void parameterAdded(const QString& label, const QColor& color);
	void newParameterValueAdded(QString label, QVariant value, QVariant timeStamp);

private slots:
	void onNewParameterAdded(BoardParameterSingle* parameter);

private:
	DataPlayer* m_dataPlayer = nullptr;
	QMetaObject::Connection m_playConnection;
	QMetaObject::Connection m_stopConnection;
	QList<QtCharts::QChart*> m_charts;
	QList<QList<QString>> m_series;
	QList<int> m_depths;

	// Вспомогательные методы
	QtCharts::QChart* findChart(const QString &label);
	int findChartIndex(const QString &label) const;
	QtCharts::QLineSeries* getOrCreateSeries(QtCharts::QChart* chart, const QString &parameterLabel);
	void setupChart(QtCharts::QChart* chart, const QString &label);
	bool parameterExistsInHistory(const QString &label) const;
};

#endif // CHARTVIEWMODEL_H
