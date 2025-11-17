#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include <QListView>

class DebugView : public QListView
{
	Q_OBJECT
public:
	explicit DebugView(QWidget *parent = nullptr);
};

#endif // DEBUGVIEW_H

