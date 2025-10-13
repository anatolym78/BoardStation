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

    // Методы для работы с приложением
    void setApp(BoardStationApp *app); 
    BoardStationApp* app() { return m_app; }
    QQmlContext* context() { return m_context; }

public:
    /// <summary>
    /// Отправка всех параметров борту (нажание на кнопку в панели урпавления дроном)
    /// </summary>
    void sendParametersToBoard();
    Q_INVOKABLE void changeSession(int sessionId);
    Q_INVOKABLE void loadSession(int sessionId);
    void switchToSession(int sessionIndex);
    void switchToLiveSession();
    Q_INVOKABLE bool saveLiveData();

private:
    BoardStationApp *m_app;
    QQmlContext *m_context;
};

#endif // QMLMAINWINDOW_H
