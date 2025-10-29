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
#include <QtCharts/QDateTimeAxis>
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
		ColorRole,
		seriesMapRole,
	};
	struct ChartInfo
	{
		QStringList series;
		QColor color = Qt::darkGray;
		bool isSelected = false;
		QMap<QString, QPointer<QtCharts::QLineSeries>> seriesMap;
		QPointer<QtCharts::QDateTimeAxis> timeAxis = nullptr;
		QPointer<QtCharts::QValueAxis> valueAxis = nullptr;
	};

	explicit ChatViewGridModel(QObject *parent = nullptr);

	void setPlayer(DataPlayer* dataPlayer);
	void setStorage(BoardParameterHistoryStorage* pStorage);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	// Методы для работы с графиками
	Q_INVOKABLE void toggleParameter(const QString& label, const QColor& color = Qt::red);
	Q_INVOKABLE void addSeries(const QString &label, const QColor& color = Qt::red);
	Q_INVOKABLE void removeSeries(const QString& label);
	Q_INVOKABLE void clearCharts();
	Q_INVOKABLE QStringList getChartSeriesLabels(int chartIndex) const;

	/// <summary>
	/// Вызывается по кнопке объединения графиков
	/// Подготавливает названия серий, который должны быть перемещены
	/// Вызывает сигнал, обрабатываемый на стороне qml, в который передеются
	/// индекс графика, в которй нужно переместить серии и названия этих серий
	/// </summary>
	/// <returns></returns>
	Q_INVOKABLE void mergeSelectedCharts();
	QList<int> selectedIndices() const;
	int countSelectedIndices() const;
	int countEmptyCharts() const;
	int firstEmptyChart() const;
	Q_INVOKABLE void splitSeries(int chartIndex);
	void removeEmptyCharts();
	
	Q_INVOKABLE QStringList chartLabels() const;

	Q_INVOKABLE bool hasSeries(const QString &label) const;
	Q_INVOKABLE int countSeries() const { return m_charts.count(); }
	Q_INVOKABLE bool isEmpty() const { return m_charts.isEmpty(); }

	Q_INVOKABLE bool selectElement(int index, bool keepSelection);
	Q_INVOKABLE void clearSelection();

	Q_INVOKABLE bool hoverElement(int index);
	Q_INVOKABLE void clearHover();

	Q_INVOKABLE bool isCanMergeCharts() const;

	// new (move charts logic to c++ code)
	Q_INVOKABLE void addSeriesToChart(int chartIndex, const QString& label, const QColor& color, QtCharts::QLineSeries* series, QtCharts::QDateTimeAxis* timeAxis, QtCharts::QValueAxis* valueAxis);
	Q_INVOKABLE void moveSeriesToChart(int chartIndex, const QString& label, QtCharts::QLineSeries* series);
	
	Q_INVOKABLE bool isSeriesCreated(const QString& label) const;

	void updateAllCells();

	void fillSeries(const QString& label, QColor color, bool isInitialFill);
	QColor labelColor(QString label);
	
signals:
	void parameterAdded(int chartIndex, const QString& label, const QColor& color);
	void parametersNeedToMove(int chartIndex, QStringList labels);
	void parameterNeedToRemove(int chartIndex, const QString& label);
	void isCanMergeChartsChanged();

private:
	DataPlayer* m_dataPlayer = nullptr;
	BoardParameterHistoryStorage* m_pStorage = nullptr;
	QMetaObject::Connection m_playConnection;
	//QMetaObject::Connection m_stopConnection;
	QList<ChartInfo> m_charts;
	//QList<QStringList> m_series;
	//QList<int> m_depths;
	//QList<bool> m_selectedIndices;
	int m_hoverIndex = -1;

	// Вспомогательные методы
	int findChartIndex(const QString &label) const;
	bool parameterExistsInHistory(const QString &label) const;
	void updateValueAxisRange(int chartIndex);

private:
	DataPlayer* player() const { return m_dataPlayer; }
	BoardParameterHistoryStorage* storage() const { return m_pStorage; }

private:
	void onParameterPlayed(BoardParameterSingle* parameter, bool isBackPlaying);
	const qint64 minuteIntervalMsec() { return 60 * 1000; }
};

#endif // CHATVIEWGRIDMODEL_H
