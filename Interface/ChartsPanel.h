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
	void onRowsInserted(const QModelIndex& parent, int first, int last);
	void onChartAdded(const QString& chartName);

private:
    ChatViewGridModel* m_chartsModel;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QGridLayout* m_gridLayout;
};

#endif // CHARTSPANEL_H
