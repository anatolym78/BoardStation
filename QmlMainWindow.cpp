#include "QmlMainWindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include "Model/Parameters/BoardParameterValue.h"
#include "ViewModel/ChartViewModel.h"
#include "ViewModel/ChatViewGridModel.h"
#include "ViewModel/SessionsListModel.h"
#include "ViewModel/SessionPlayer.h"
#include <QQuickItem>
#include <QCloseEvent>

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>

QmlMainWindow::QmlMainWindow(QWindow *parent) : QQuickView(parent), m_app(nullptr), m_context(nullptr)
{
	qmlRegisterType<BoardParameterValue>("BoardStation", 1, 0, "BoardParameterValue");
	qmlRegisterType<BoardParameterSingle>("BoardStation", 1, 0, "BoardParameterSingle");

	qmlRegisterType<ChatViewGridModel>("BoardStation", 1, 0, "ChatViewGridModel");
	qmlRegisterType<SessionsListModel>("BoardStation", 1, 0, "SessionsListModel");
	qmlRegisterType<SessionPlayer>("BoardStation", 1, 0, "SessionPlayer");
	
	setResizeMode(ResizeMode::SizeRootObjectToView);
	
	m_context = rootContext();
	
	m_context->setContextProperty("qmlMainWindow", this);
}

void QmlMainWindow::setApp(BoardStationApp *pApp)
{
	m_app = pApp;
	
	if (pApp)
	{       
		if (context())
		{
			context()->setContextProperty(QString("parametersListModel"), app()->parametersModel());
		
			context()->setContextProperty("uplinkParametersModel", app()->getUplinkParametersModel());

			context()->setContextProperty("debugViewModel", app()->getDebugViewModel());

			context()->setContextProperty("sessionsListModel", app()->getSessionsListModel());

			context()->setContextProperty("parametersPlayer", app()->player());

			context()->setContextProperty("boardMessagesWriter", app()->getBoardMessagesWriter());
			
			context()->setContextProperty("chartViewModel", app()->getChartViewModel());
		}
		
		setSource(QUrl("qrc:/Interface/Qml/main.qml"));
		
		if (status() == QQuickView::Error) 
		{
			qWarning() << "QML errors:" << errors();
		}
	}
}

void QmlMainWindow::sendParametersToBoard()
{
	if (app())
	{
		app()->sendParametersToBoard();

	}
}

void QmlMainWindow::changeSession()
{
	if (app())
	{
		context()->setContextProperty(QString("parametersPlayer"), app()->player());
		context()->setContextProperty(QString("parametersListModel"), app()->parametersModel());
		context()->setContextProperty(QString("chartViewModel"), app()->getChartViewModel());
	}
}

bool QmlMainWindow::saveLiveData()
{
	return app()->saveLiveData();
}

QmlMainWindow::~QmlMainWindow()
{
}
