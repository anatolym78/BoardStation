#ifndef SESSIONLISTVIEW_H
#define SESSIONLISTVIEW_H

#include <QListView>

class SessionListView : public QListView
{
    Q_OBJECT
public:
    explicit SessionListView(QWidget *parent = nullptr);

public:
	void createSelectionModel();


signals:
    void sessionSelected(int sessionIndex);

private:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
};

#endif // SESSIONLISTVIEW_H

