#ifndef CHARTSPANEL_H
#define CHARTSPANEL_H

#include <QFrame>
#include <QList>
#include <QMap>
#include "./ViewModel/ChatViewGridModel.h"
#include "ParametersChartView.h"
#include "./../Model/Parameters/Tree/ParameterTreeHistoryItem.h"

class QScrollArea;
class QGridLayout;
class QWidget;
class QPushButton;

class ChartsPanel : public QFrame
{
	Q_OBJECT
public:
	explicit ChartsPanel(QWidget *parent = nullptr);
	void setModel(ChatViewGridModel* chartsModel);

	void onParameterItemHovered(ParameterTreeHistoryItem* treeItem);

protected:
	void onAddChart(int chartIndex, ParameterTreeItem* parameter);
	void onParameterRemoved(int chartIndex, const QString& label);
	void onParameterMoved(int chartIndex, const QStringList& labels);
	bool eventFilter(QObject* watched, QEvent* event) override;

	ParametersChartView* getChartView(int chartIndex);

private:
	void updateCellSizes();
	void relayoutChartsGrid();

	void onOneColumnClicked();
	void onTwoColumnClicked();
	void onMergeChartsClicked();

	ChatViewGridModel* m_chartsModel;
	QScrollArea* m_scrollArea;
	QWidget* m_scrollContent;
	QGridLayout* m_gridLayout;
	QPushButton* m_oneColumnButton;
	QPushButton* m_twoColumnButton;
	int m_columnCount = 2;
	QMap<QtCharts::QAbstractSeries*, QColor>  m_seriesColors;

private:
	void addSeriesToChart(int chartIndex, QtCharts::QChart* chart, ParameterTreeItem* parameter);
	QList<ParametersChartView*> chartViewList() const;
	void hoverSeries(QtCharts::QAbstractSeries* series);
	void restoreSeriesColor(QtCharts::QAbstractSeries* series);
};

#endif // CHARTSPANEL_H
