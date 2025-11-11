#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterHistory.h"
#include "Interface/Charts/ChartBuilder.h"
#include "Interface/ParametersTreeView.h"
#include "./Interface/SessionFrame.h"
#include "./ViewModel/SessionsListModel.h"

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
{
	ui->setupUi(this);

	connect(ui->buttonSaveRecord, &QPushButton::clicked, this, &MainWindow::saveRecord);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setApp(BoardStationApp *pApp)
{
	m_app = pApp;
	
	if (m_app)
	{
		auto sessions = pApp->sessionsModel();
		for (auto i = 0; i < sessions->rowCount(); i++)
		{
			auto sessionFrame = new SessionFrame(sessions->session(i), this);
			sessionsStack()->addWidget(sessionFrame);
		}

		sessionsListView()->setModel(pApp->sessionsModel());
		
		qDebug() << "MainWindow: Приложение установлено";
	}
}

BoardStationApp* MainWindow::app() const
{
	return m_app;
}

void MainWindow::saveRecord()
{
	app()->saveLiveData();
}

SessionsStackWidget *MainWindow::sessionsStack() const
{
	return ui->sessionsStackWidget;
}

SessionListView* MainWindow::sessionsListView() const
{
	return ui->listView;
}



