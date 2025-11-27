#ifndef SESSIONSSTACKWIDGET_H
#define SESSIONSSTACKWIDGET_H

#include <QStackedWidget>

#include "SessionFrame.h"

class SessionsStackWidget : public QStackedWidget
{
	Q_OBJECT
public:
	explicit SessionsStackWidget(QWidget *parent = nullptr);
	void setCurrentIndex(int index);
	SessionFrame* getSessionFrame(int index) const;

signals:
};

#endif // SESSIONSSTACKWIDGET_H
