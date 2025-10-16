#ifndef QMLMAINWINDOW_H
#define QMLMAINWINDOW_H

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include "BoardStationApp.h"

class QmlMainWindow : public QQuickView
{
    Q_OBJECT

public:
    QmlMainWindow(QWindow *parent = nullptr);
    ~QmlMainWindow();

    void setApp(BoardStationApp *app); 
    BoardStationApp* app() { return m_app; }
    QQmlContext* context() { return m_context; }

public:
    void sendParametersToBoard();
    Q_INVOKABLE void changeSession();
    Q_INVOKABLE bool saveLiveData();

private:
    BoardStationApp *m_app;
    QQmlContext *m_context;
};

#endif // QMLMAINWINDOW_H
