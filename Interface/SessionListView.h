#ifndef SESSIONLISTVIEW_H
#define SESSIONLISTVIEW_H

#include <QListView>

class SessionListView : public QListView
{
    Q_OBJECT
public:
    explicit SessionListView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawHeader(QPainter *painter);
    int m_headerHeight;
};

#endif // SESSIONLISTVIEW_H

