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
    BoardStationApp* getApp() const;

public slots:
    void sendParametersToBoard();
    void loadSession(int sessionId);
    void switchToSession(int sessionIndex);
    void switchToLiveSession();
    bool saveLiveData();

private slots:
    void onDataSent(const QString &data);

private:
    void setupModel();
    void setupUplinkParametersModel();
    void setupDebugViewModel();
    void setupChartSeriesModel();
    void setupSessionsListModel();
    void addTestDataToModel();

private:
    BoardStationApp *m_app;
    QQmlContext *m_context;
};

#endif // QMLMAINWINDOW_H
