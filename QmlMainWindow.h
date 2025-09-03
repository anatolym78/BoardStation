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

private slots:
    void onParameterUpdated(const QString &label);

private:
    void setupModel();
    void setupOutParametersModel();
    void setupChartSeriesModel();
    void setupConnections();

private:
    BoardStationApp *m_app;
    QQmlContext *m_context;
};

#endif // QMLMAINWINDOW_H
