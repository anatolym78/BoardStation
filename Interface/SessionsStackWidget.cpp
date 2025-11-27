#include "SessionsStackWidget.h"

SessionsStackWidget::SessionsStackWidget(QWidget *parent)
    : QStackedWidget(parent)
{
    connect(this, &QStackedWidget::currentChanged, [=](int index)
        {
            int k = 0;
        });
}

void SessionsStackWidget::setCurrentIndex(int index)
{
    QStackedWidget::setCurrentIndex(index);
}

SessionFrame* SessionsStackWidget::getSessionFrame(int index) const
{
    auto widget = this->widget(index);

    if (widget == nullptr) return nullptr;

    return (SessionFrame*)widget;
}
