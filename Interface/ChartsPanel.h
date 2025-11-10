#ifndef CHARTSPANEL_H
#define CHARTSPANEL_H

#include <QFrame>
#include "./ViewModel/ChatViewGridModel.h"

class QScrollArea;
class QGridLayout;
class QWidget;

class ChartsPanel : public QFrame
{
	Q_OBJECT
public:
	explicit ChartsPanel(QWidget *parent = nullptr);
	void setModel(ChatViewGridModel* chartsModel);

protected:
	void onParameterAdded(int chartIndex, ParameterTreeItem* parameter, const QColor& color);
	void onParameterRemoved(int chartIndex, const QString& label);
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void updateCellSizes();

	ChatViewGridModel* m_chartsModel;
	QScrollArea* m_scrollArea;
	QWidget* m_scrollContent;
	QGridLayout* m_gridLayout;
	int m_columnCount = 2;
};

#endif // CHARTSPANEL_H
