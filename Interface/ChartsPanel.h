#ifndef CHARTSPANEL_H
#define CHARTSPANEL_H

#include <QFrame>

class QScrollArea;
class QGridLayout;
class QWidget;

class ChartsPanel : public QFrame
{
    Q_OBJECT
public:
    explicit ChartsPanel(QWidget *parent = nullptr);

private:
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QGridLayout* m_gridLayout;
};

#endif // CHARTSPANEL_H
