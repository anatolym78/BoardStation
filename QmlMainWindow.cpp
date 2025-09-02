#include "QmlMainWindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameter.h"

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>

QmlMainWindow::QmlMainWindow(QWindow *parent)
    : QQuickView(parent)
    , m_app(nullptr)
    , m_context(nullptr)
{
    // Configure QML engine
    setResizeMode(QQuickView::SizeRootObjectToView);
    
    // Create context
    m_context = rootContext();
    
    qDebug() << "QmlMainWindow: QML interface loaded";
}

QmlMainWindow::~QmlMainWindow()
{
}

void QmlMainWindow::setApp(BoardStationApp *app)
{
    m_app = app;
    
    if (m_app) {
        // Notify application about main window
        m_app->setMainWindow(nullptr); // QML window doesn't inherit from QMainWindow
        
        // Setup model
        setupModel();
        
        // Setup out parameters model
        setupOutParametersModel();
        
        // Connect parameter update signals
        if (m_app->getParametersStorage()) {
            connect(m_app->getParametersStorage(), &BoardParametersStorage::parameterUpdated,
                    this, &QmlMainWindow::onParameterUpdated);
        }
        
        // Load QML file after setting up models
        setSource(QUrl("qrc:/Interface/Qml/main.qml"));
        
        if (status() == QQuickView::Error) {
            qWarning() << "QML errors:" << errors();
        }
        
        qDebug() << "QmlMainWindow: Application set";
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
    auto parametersModel = m_app->getParametersModel();
    if (parametersModel) {
        // Pass model to QML context
        m_context->setContextProperty("parametersModel", parametersModel);
        qDebug() << "QmlMainWindow: Parameters model passed to QML";
        qDebug() << "QmlMainWindow: Number of rows in model:" << parametersModel->rowCount();
    } else {
        qDebug() << "QmlMainWindow: Parameters model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty("parametersModel", nullptr);
    }
    
    // Initialize lastUpdatedParameter
    m_context->setContextProperty("lastUpdatedParameter", "");
}

void QmlMainWindow::setupOutParametersModel()
{
    if (!m_app || !m_context) return;
    
    // Get out parameters model from application
    auto outParametersModel = m_app->getOutParametersModel();
    if (outParametersModel) {
        // Pass model to QML context
        m_context->setContextProperty("outParametersModel", outParametersModel);
        qDebug() << "QmlMainWindow: Out parameters model passed to QML";
    } else {
        qDebug() << "QmlMainWindow: Out parameters model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty("outParametersModel", nullptr);
    }
    
    // Добавляем объект QmlMainWindow в QML контекст для вызова методов
    m_context->setContextProperty("qmlMainWindow", this);
}

void QmlMainWindow::setupConnections()
{
    // Here you can setup additional connections between QML and C++
    qDebug() << "QmlMainWindow: Connections set up";
}

void QmlMainWindow::onParameterUpdated(const QString &label)
{
    // Update QML interface when parameter changes
    qDebug() << "QmlMainWindow: Parameter updated:" << label;
    
    // Can send signal to QML to update interface
    if (m_context) {
        m_context->setContextProperty("lastUpdatedParameter", label);
    }
}

void QmlMainWindow::sendParametersToBoard()
{
    qDebug() << "QmlMainWindow: Send parameters to board requested from QML";
    
    if (m_app) {
        m_app->sendParametersToBoard();
        qDebug() << "QmlMainWindow: Parameters sent to board successfully";
    } else {
        qWarning() << "QmlMainWindow: Application instance is not available";
    }
}
