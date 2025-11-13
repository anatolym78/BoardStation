#ifndef CHARTSPANEL_H
#define CHARTSPANEL_H

#include <QFrame>
#include "./ViewModel/ChatViewGridModel.h"
#include "ParametersChartView.h"

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

protected:
	void onParameterAdded(int chartIndex, ParameterTreeItem* parameter, const QColor& color);
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
};

#endif // CHARTSPANEL_H
