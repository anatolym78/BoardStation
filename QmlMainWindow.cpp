#include "QmlMainWindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterHistory.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include "Model/Parameters/BoardParameterValue.h"
#include "ViewModel/ChartViewModel.h"
#include "ViewModel/SessionsListModel.h"
#include "ViewModel/SessionPlayer.h"
#include <QQuickItem>

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>

QmlMainWindow::QmlMainWindow(QWindow *parent)
    : QQuickView(parent)
    , m_app(nullptr)
    , m_context(nullptr)
{
    // Регистрируем типы для QML
    qmlRegisterType<BoardParameterValue>("BoardStation", 1, 0, "BoardParameterValue");
    qmlRegisterType<BoardParameterSingle>("BoardStation", 1, 0, "BoardParameterSingle");
    qmlRegisterType<BoardParameterHistory>("BoardStation", 1, 0, "BoardParameterHistory");

    qmlRegisterType<ChartViewModel>("BoardStation", 1, 0, "ChartViewModel");
    qmlRegisterType<SessionsListModel>("BoardStation", 1, 0, "SessionsListModel");
    qmlRegisterType<SessionPlayer>("BoardStation", 1, 0, "SessionPlayer");
    
    // Configure QML engine
    setResizeMode(SizeRootObjectToView);
    
    // Create context
    m_context = rootContext();
    
    m_context->setContextProperty("qmlMainWindow", this);

    //qDebug() << "QmlMainWindow: QML interface loaded";
}

QmlMainWindow::~QmlMainWindow()
{
}

void QmlMainWindow::setApp(BoardStationApp *app)
{
    m_app = app;
    
    if (m_app)
    {
        // Notify application about main window
        //m_app->setMainWindow(nullptr); // QML window doesn't inherit from QMainWindow
        
        // Setup model
        setupModel();
        
        // Setup uplink parameters model
        setupUplinkParametersModel();
        
        // Setup debug view model
        setupDebugViewModel();
        
        // Setup chart series model
        setupChartSeriesModel();
        
        // Setup sessions list model
        setupSessionsListModel();
        
        // Connect driver signals
        if (m_app->getDriver())
        {
            connect(m_app->getDriver(), &drv::IDriver::dataSent,
                    this, &QmlMainWindow::onDataSent);
        }
        
        // Load QML file after setting up models
        setSource(QUrl("qrc:/Interface/Qml/main.qml"));
        
        if (status() == QQuickView::Error) {
            qWarning() << "QML errors:" << errors();
        }
        
        //qDebug() << "QmlMainWindow: Application set";
    }
}

BoardStationApp* QmlMainWindow::getApp() const
{
    return m_app;
}

void QmlMainWindow::setupModel()
{
    if (!m_app || !m_context) return;
    
    // Get model from application
    auto parametersListModel = m_app->getParametersModel();
    if (parametersListModel)
    {
        // Pass model to QML context
        m_context->setContextProperty(QString("parametersListModel"), parametersListModel);
        //qDebug() << "QmlMainWindow: Parameters model passed to QML";
        //qDebug() << "QmlMainWindow: Number of rows in model:" << parametersModel->rowCount();
        
        // Добавляем тестовые данные для проверки
        addTestDataToModel();
    } 
    else 
    {
        //qDebug() << "QmlMainWindow: Parameters model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty(QString("parametersListModel"), nullptr);
    }
   
}

void QmlMainWindow::setupUplinkParametersModel()
{
    if (!m_app || !m_context) return;
    
    // Get uplink parameters model from application
    auto uplinkParametersModel = m_app->getUplinkParametersModel();
    if (uplinkParametersModel) 
    {
        // Pass model to QML context
        m_context->setContextProperty("uplinkParametersModel", uplinkParametersModel);
        //qDebug() << "QmlMainWindow: Uplink parameters model passed to QML";
    }
    else 
    {
        //qDebug() << "QmlMainWindow: Uplink parameters model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty("uplinkParametersModel", nullptr);
    }
}

void QmlMainWindow::setupDebugViewModel()
{
    if (!m_app || !m_context) return;
    
    // Get debug view model from application
    auto debugViewModel = m_app->getDebugViewModel();
    if (debugViewModel) 
    {
        // Pass model to QML context
        m_context->setContextProperty("debugViewModel", debugViewModel);
        //qDebug() << "QmlMainWindow: Debug view model passed to QML";
    }
    else 
    {
        //qDebug() << "QmlMainWindow: Debug view model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty("debugViewModel", nullptr);
    }
}

void QmlMainWindow::setupChartSeriesModel()
{
    if (!m_app || !m_context) return;
    
    //// Create new simplified chart view model
    //auto chartViewModel = m_app->getChartViewModel();
    //chartViewModel->setParametersStorage(m_app->getParametersStorage());
    //
    //// Pass model to QML context
    //m_context->setContextProperty("chartViewModel", chartViewModel);
}

void QmlMainWindow::setupSessionsListModel()
{
    if (!m_app || !m_context) return;
    
    // Get sessions list model from application
    auto sessionsListModel = m_app->getSessionsListModel();
    if (sessionsListModel) 
    {
        // Pass model to QML context
        m_context->setContextProperty("sessionsListModel", sessionsListModel);
        qDebug() << "QmlMainWindow: Sessions list model set from application";
    }
    else 
    {
        qWarning() << "QmlMainWindow: Sessions list model not found in application";
    }
    
    auto dataPlayer = m_app->getDataPlayer();
    //// Create session player
    //auto sessionPlayer = m_app->getSessionPlayer();
    //auto currentPlayer = m_app->getCurrentPlayer();
    //
    //// Pass players to QML context
    //m_context->setContextProperty("sessionPlayer", sessionPlayer);
    m_context->setContextProperty("driverDataPlayer", dataPlayer);
    
    // Pass board messages writer to QML context
    auto boardMessagesWriter = m_app->getBoardMessagesWriter();
    if (boardMessagesWriter)
    {
        m_context->setContextProperty("boardMessagesWriter", boardMessagesWriter);
    }
    
    qDebug() << "QmlMainWindow: Session player and board messages writer created and added to context";
}

void QmlMainWindow::onDataSent(const QString &data)
{
    //qDebug() << "QmlMainWindow: Data sent to board:" << data;
    
    // Добавляем сообщение в модель отладки
    if (m_app && m_app->getDebugViewModel())
    {
        m_app->getDebugViewModel()->addUplinkParametersMessage(data);
    }
}

void QmlMainWindow::addTestDataToModel()
{
    return;
}

void QmlMainWindow::sendParametersToBoard()
{
    //qDebug() << "QmlMainWindow: Send parameters to board requested from QML";
    
    if (m_app)
    {
        m_app->sendParametersToBoard();
        //qDebug() << "QmlMainWindow: Parameters sent to board successfully";
    } else 
    {
        qWarning() << "QmlMainWindow: Application instance is not available";
    }
}

void QmlMainWindow::loadSession(int sessionId)
{
    if (m_app)
    {
        m_app->loadSession(sessionId);
        qDebug() << "QmlMainWindow: Loading session" << sessionId;
    }
    else
    {
        qWarning() << "QmlMainWindow: Application instance is not available";
    }
}

void QmlMainWindow::switchToSession(int sessionIndex)
{
    if (m_app)
    {
        m_app->switchToSession(sessionIndex);
        
        // Обновляем плеер в QML контексте
        auto dataPlayer = m_app->getDataPlayer();
        if (dataPlayer && m_context)
        {
            m_context->setContextProperty("driverDataPlayer", dataPlayer);
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
    if (m_app)
    {
        m_app->switchToLiveSession();
        
        // Обновляем плеер в QML контексте
        auto dataPlayer = m_app->getDataPlayer();
        if (dataPlayer && m_context)
        {
            m_context->setContextProperty("driverDataPlayer", dataPlayer);
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
