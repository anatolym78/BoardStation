#include "ChartsPanel.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>

ChartsPanel::ChartsPanel(QWidget *parent)
    : QFrame(parent)
{
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    //m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_scrollContent = new QWidget();
    m_gridLayout = new QGridLayout(m_scrollContent);
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);

    m_scrollArea->setWidget(m_scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}
