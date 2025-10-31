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
	qmlRegisterType<BoardParametersListModel>("BoardStation", 1, 0, "BoardParametersListModel");
	qmlRegisterType<BoardParametersTreeModel>("BoardStation", 1, 0, "BoardParametersTreeModel");

	qmlRegisterType<ChatViewGridModel>("BoardStation", 1, 0, "ChatViewGridModel");
	qmlRegisterType<SessionsListModel>("BoardStation", 1, 0, "SessionsListModel");

	qmlRegisterUncreatableType<DataPlayer>("BoardStation", 1, 0, "DataPlayer",
		"DataPlayer cannot be created in QML");
	qmlRegisterUncreatableType<SessionPlayer>("BoardStation", 1, 0, "SessionPlayer",
		"SessionPlayer cannot be created in QML");
	qmlRegisterUncreatableType<DriverDataPlayer>("BoardStation", 1, 0, "DriverDataPlayer",
		"DriverDataPlayer cannot be created in QML");
	
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
			context()->setContextProperty("sessionsListModel", app()->getSessionsListModel());
				
			context()->setContextProperty("uplinkParametersModel", app()->getUplinkParametersModel());

			context()->setContextProperty("debugViewModel", app()->getDebugViewModel());

			context()->setContextProperty("boardMessagesWriter", app()->getBoardMessagesWriter());	
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
	return;
}

bool QmlMainWindow::saveLiveData()
{
	return app()->saveLiveData();
}

QmlMainWindow::~QmlMainWindow()
{
}
