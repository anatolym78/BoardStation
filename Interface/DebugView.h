#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include <QTableView>

class QAbstractItemModel;

class DebugView : public QTableView
{
	Q_OBJECT
public:
	explicit DebugView(QWidget *parent = nullptr);
	
	void setModel(QAbstractItemModel *model) override;

private:
	void setupColumnWidths();
};

#endif // DEBUGVIEW_H

