#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMap>
#include <QChartView>
#include "ViewModel/OutParametersModel.h"

class BoardStationApp;
class ChartBuilder;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Методы для работы с приложением
    void setApp(BoardStationApp *app);
    BoardStationApp* getApp() const;
    
    // Метод для загрузки сессии (для QML)
    //Q_INVOKABLE void loadSession(int sessionId);

// private slots:
//     void onSendToBoardButtonClicked();
//     void onClearDebugButtonClicked();
//     void onParameterDoubleClicked(const QModelIndex &index);
//     void onParameterUpdated(const QString &label);
//     void onChartWindowClosed(const QString &parameterName);
//     void onSendImmediatelyCheckBoxChanged(int state);

// private:
//     void setupModel();
//     void setupOutParametersModel();
//     void setupDockWidgets();
//     void setupConnections();
//     void createChartWindow(const QString &parameterName);
//     void updateChart(const QString &parameterName);

private:
    Ui::MainWindow *ui;
    BoardStationApp *m_app;
};

#endif // MAINWINDOW_H
