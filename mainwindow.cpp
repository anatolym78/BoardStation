#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterHistory.h"
#include "Interface/Charts/ChartBuilder.h"

#include <QDebug>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QTextEdit>
#include <QMessageBox>
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_app(nullptr)
    // , m_chartBuilder(new ChartBuilder(this))
    // , m_outParametersModel(nullptr)
{
    ui->setupUi(this);
    // setupDockWidgets();
    // setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setApp(BoardStationApp *app)
{
    m_app = app;
    
    if (m_app)
    {
        // Уведомляем приложение о главном окне
        //m_app->setMainWindow(this);
        
        // // Настраиваем модель
        // setupModel();
        
        // // Настраиваем модель исходящих параметров
        // setupOutParametersModel();
       
        
        // // Подключаем сигналы обновления параметров для обновления графиков
        // if (m_app->getParametersStorage())
        // {
        //     connect(m_app->getParametersStorage(), &BoardParameterHistoryStorage::parameterUpdated,
        //             this, &MainWindow::onParameterUpdated);
        // }
        
        qDebug() << "MainWindow: Приложение установлено";
    }
}

BoardStationApp* MainWindow::getApp() const
{
    return m_app;
}

// void MainWindow::setupDockWidgets()
// {
//     // Программно устанавливаем области размещения для QDockWidget
//     addDockWidget(Qt::LeftDockWidgetArea, ui->parametersDockWidget);
//     addDockWidget(Qt::RightDockWidgetArea, ui->controlDockWidget);
//     addDockWidget(Qt::BottomDockWidgetArea, ui->debugDockWidget);
    
//     // Группируем левые панели
//     // tabifyDockWidget(ui->parametersDockWidget, ui->controlDockWidget);
//     ui->parametersDockWidget->raise();
    
//     qDebug() << "MainWindow: Dock widgets настроены";
// }

// void MainWindow::setupConnections()
// {
//     // Подключаем сигналы кнопок
//     connect(ui->sendToBoardButton, &QPushButton::clicked, this, &MainWindow::onSendToBoardButtonClicked);
//     connect(ui->clearDebugButton, &QPushButton::clicked, this, &MainWindow::onClearDebugButtonClicked);
    
//     // Подключаем сигнал изменения флажка
//     connect(ui->sendImmediatelyCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onSendImmediatelyCheckBoxChanged);
    
//     // Подключаем сигнал двойного клика на таблицу параметров
//     connect(ui->parametersTableView, &QTableView::clicked, this, &MainWindow::onParameterDoubleClicked);
    
//     qDebug() << "MainWindow: Соединения настроены";
// }

// void MainWindow::setupModel()
// {
//     if (!m_app) return;
    
//     // Получаем модель из приложения и связываем с QTableView
//     auto parametersModel = m_app->getParametersModel();
//     if (parametersModel) {
//         ui->parametersTableView->setModel(parametersModel);
        
//         // Настраиваем делегат для отображения параметров
//         ui->parametersTableView->setItemDelegate(new QStyledItemDelegate());
        
//         // Устанавливаем размер строк
//         //ui->parametersTableView->setUniformRowHeights(false);
        
//         // Настраиваем заголовки колонок
//         ui->parametersTableView->horizontalHeader()->setStretchLastSection(true);
//         ui->parametersTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        
//         qDebug() << "MainWindow: Модель параметров успешно связана с QTableView";
//     }
// }

// void MainWindow::setupOutParametersModel()
// {
//     if (!m_app) return;
    
//     // Получаем модель исходящих параметров из приложения и связываем с QTableView
//     auto outParametersModel = m_app->getOutParametersModel();
//     if (outParametersModel) {
//         ui->enginesTableView->setModel(outParametersModel);
        
//         // Настраиваем заголовки колонок
//         ui->enginesTableView->horizontalHeader()->setStretchLastSection(true);
//         ui->enginesTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//         ui->enginesTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        
//         qDebug() << "MainWindow: Модель исходящих параметров успешно связана с QTableView";
//     }
// }

// void MainWindow::onSendToBoardButtonClicked()
// {
//     qDebug() << "MainWindow: Кнопка Send to drone нажата";
//     ui->debugTextEdit->append(tr("Send to drone button clicked"));
    
//     if (m_app) {
//         // Отправляем параметры на борт
//         m_app->sendParametersToBoard();
//         ui->debugTextEdit->append(tr("Parameters sent to board"));
//     } else {
//         qWarning() << "MainWindow: Application instance is not available";
//         ui->debugTextEdit->append(tr("Error: Application not available"));
//     }
// }

// void MainWindow::onClearDebugButtonClicked()
// {
//     ui->debugTextEdit->clear();
//     qDebug() << "MainWindow: Отладочная панель очищена";
// }

// void MainWindow::onSendImmediatelyCheckBoxChanged(int state)
// {
//     bool isChecked = (state == Qt::Checked);
//     qDebug() << "MainWindow: Флажок 'Отправлять сразу' изменен на:" << isChecked;
    
//     if (isChecked) {
//         ui->debugTextEdit->append(tr("Auto-send enabled"));
//     } else {
//         ui->debugTextEdit->append(tr("Auto-send disabled"));
//     }
// }

// void MainWindow::onParameterDoubleClicked(const QModelIndex &index)
// {
//     if (!index.isValid()) return;
    
//     // Получаем название параметра из первой колонки
//     QString parameterName = index.sibling(index.row(), 0).data().toString();
//     qDebug() << "MainWindow: Двойной клик на параметр:" << parameterName;
    
//     // Создаем окно с графиком
//     createChartWindow(parameterName);
// }

// void MainWindow::createChartWindow(const QString &parameterName)
// {
//     // Проверяем, есть ли уже график для этого параметра
//     if (m_chartViews.contains(parameterName)) {
//         // Если график уже существует, скрываем его и удаляем
//         QChartView *existingChartView = m_chartViews[parameterName];
//         if (existingChartView->parent()) {
//             QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow*>(existingChartView->parent());
//             if (subWindow) {
//                 // Закрываем окно
//                 subWindow->close();
//                 // Удаляем из словаря
//                 m_chartViews.remove(parameterName);
//                 qDebug() << "MainWindow: Закрыто окно с графиком для параметра:" << parameterName;
//                 return;
//             }
//         }
//     }
    
//     // Получаем данные параметра из хранилища
//     if (!m_app || !m_app->getParametersStorage()) {
//         qWarning() << "MainWindow: Не удалось получить доступ к хранилищу параметров";
//         return;
//     }
    
//     // TODO: Переписать логику для работы с m_sessionParameters вместо BoardParameterHistory
//     qWarning() << "MainWindow: Метод createChartWindow требует переработки для работы с новой архитектурой";
//     return;
// }

// void MainWindow::onParameterUpdated(const QString &label)
// {
//     // Обновляем график, если он существует для этого параметра
//     if (m_chartViews.contains(label)) {
//         // Дополнительная проверка: график все еще существует
//         QChartView *chartView = m_chartViews[label];
//         if (chartView && chartView->chart()) {
//             updateChart(label);
//         } else {
//             // График был удален, убираем из словаря
//             m_chartViews.remove(label);
//             qDebug() << "MainWindow: График был удален в onParameterUpdated:" << label;
//         }
//     }
// }

// void MainWindow::updateChart(const QString &parameterName)
// {
//     if (!m_chartViews.contains(parameterName) || !m_app || !m_app->getParametersStorage()) {
//         return;
//     }
    
//     QChartView *chartView = m_chartViews[parameterName];
    
//     // Дополнительная проверка: график все еще существует и не был удален
//     if (!chartView || !chartView->chart()) {
//         // График был удален, убираем из словаря
//         m_chartViews.remove(parameterName);
//         qDebug() << "MainWindow: График был удален, убираем из словаря:" << parameterName;
//         return;
//     }
    
//     // TODO: Переписать логику для работы с m_sessionParameters вместо BoardParameterHistory
//     qWarning() << "MainWindow: Метод updateChart требует переработки для работы с новой архитектурой";
// }

// void MainWindow::onChartWindowClosed(const QString &parameterName)
// {
//     // Удаляем график из словаря при закрытии окна
//     if (m_chartViews.contains(parameterName)) {
//         m_chartViews.remove(parameterName);
//         qDebug() << "MainWindow: График удален из словаря для параметра:" << parameterName;
//     }
// }

// void MainWindow::loadSession(int sessionId)
// {
//     if (m_app)
//     {
//         m_app->loadSession(sessionId);
//         qDebug() << "MainWindow: Loading session" << sessionId << "via app";
//     }
//     else
//     {
//         qWarning() << "MainWindow: App is not available";
//     }
// }


