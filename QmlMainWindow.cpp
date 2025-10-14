#include "QmlMainWindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include "Model/Parameters/BoardParameterValue.h"
#include "ViewModel/ChartViewModel.h"
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

	qmlRegisterType<ChartViewModel>("BoardStation", 1, 0, "ChartViewModel");
	qmlRegisterType<SessionsListModel>("BoardStation", 1, 0, "SessionsListModel");
	qmlRegisterType<SessionPlayer>("BoardStation", 1, 0, "SessionPlayer");
	//qmlRegisterType<Session>("BoardStation", 1, 0, "Session");
	
	setResizeMode(ResizeMode::SizeRootObjectToView);
	
	m_context = rootContext();
	
	m_context->setContextProperty("qmlMainWindow", this);

    //connect(this, &QQuickWindow::closing, this, &QmlMainWindow::onClosing);
}

void QmlMainWindow::setApp(BoardStationApp *pApp)
{
	m_app = pApp;
	
	if (pApp)
	{       
		if (context())
		{
			context()->setContextProperty(QString("parametersListModel"), app()->getParametersModel());
		
			context()->setContextProperty("uplinkParametersModel", app()->getUplinkParametersModel());

			context()->setContextProperty("debugViewModel", app()->getDebugViewModel());

			context()->setContextProperty("sessionsListModel", app()->getSessionsListModel());

			context()->setContextProperty("parametersPlayer", app()->getDataPlayer());

			context()->setContextProperty("boardMessagesWriter", app()->getBoardMessagesWriter());
		}
		
		// Load QML file after setting up models
		setSource(QUrl("qrc:/Interface/Qml/main.qml"));
		
		if (status() == QQuickView::Error) 
		{
			qWarning() << "QML errors:" << errors();
		}
	}
}

void QmlMainWindow::sendParametersToBoard()
{
	app()->sendParametersToBoard();
}


void QmlMainWindow::changeSession(int sessionId)
{
	if (!app()) return;

	auto player = app()->changeSession(sessionId);

	context()->setContextProperty("parametersPlayer", player);
}

void QmlMainWindow::switchToSession(int sessionIndex)
{
	return;

	if (m_app)
	{
		m_app->switchToSession(sessionIndex);
		
		// Обновляем плеер в QML контексте
		auto dataPlayer = m_app->getDataPlayer();
		if (dataPlayer && m_context)
		{
			m_context->setContextProperty("parametersPlayer", dataPlayer);
		}
		
		qDebug() << "QmlMainWindow: Switching to session at index" << sessionIndex;
	}
	else
	{
		qWarning() << "QmlMainWindow: Application instance is not available";
	}
}

void QmlMainWindow::switchToLiveSession()
{
	return;

	if (m_app)
	{
		m_app->switchToLiveSession();
		
		// Обновляем плеер в QML контексте
		auto dataPlayer = m_app->getDataPlayer();
		if (dataPlayer && m_context)
		{
			m_context->setContextProperty("parametersPlayer", dataPlayer);
		}
		
		qDebug() << "QmlMainWindow: Switching to live session";
	}
	else
	{
		qWarning() << "QmlMainWindow: Application instance is not available";
	}
}

bool QmlMainWindow::saveLiveData()
{
	return m_app->saveLiveData();
}


void QmlMainWindow::onClosing()
{
	app()->close();
}

QmlMainWindow::~QmlMainWindow()
{
}
