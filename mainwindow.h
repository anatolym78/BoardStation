#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMap>
#include <QStackedWidget>
#include <QChartView>
#include "ViewModel/OutParametersModel.h"
#include "Interface/SessionsStackWidget.h"
#include "Interface/SessionListView.h"

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
    BoardStationApp* app() const;
    
    void saveRecord();

private:
    Ui::MainWindow *ui;
    BoardStationApp *m_app;

private:
    SessionsStackWidget* sessionsStack() const;
    SessionListView* sessionsListView() const;
};

#endif // MAINWINDOW_H
