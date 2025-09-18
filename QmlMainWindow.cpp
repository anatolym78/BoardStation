#include "QmlMainWindow.h"
#include "BoardStationApp.h"
#include "Model/Parameters/BoardParameterHistory.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include "Model/Parameters/BoardParameterValue.h"
#include "ViewModel/ChartPointsModel.h"
#include "ViewModel/ChartSeriesModel.h"
#include "ViewModel/ChartsListModel.h"

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
    qmlRegisterType<ChartPointsModel>("BoardStation", 1, 0, "ChartPointsModel");
    qmlRegisterType<ChartSeriesModel>("BoardStation", 1, 0, "ChartSeriesModel");
    qmlRegisterType<ChartsListModel>("BoardStation", 1, 0, "ChartsListModel");
    
    // Configure QML engine
    setResizeMode(SizeRootObjectToView);
    
    // Create context
    m_context = rootContext();
    
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
        m_app->setMainWindow(nullptr); // QML window doesn't inherit from QMainWindow
        
        // Setup model
        setupModel();
        
        // Setup out parameters model
        setupOutParametersModel();
        
        // Setup chart series model
        setupChartSeriesModel();
        
        // Connect parameter update signals
        if (m_app->getParametersStorage())
        {
            connect(m_app->getParametersStorage(), &BoardParameterHistoryStorage::parameterUpdated,
                    this, &QmlMainWindow::onParameterUpdated);
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

void QmlMainWindow::setupOutParametersModel()
{
    if (!m_app || !m_context) return;
    
    // Get out parameters model from application
    auto outParametersModel = m_app->getOutParametersModel();
    if (outParametersModel) 
    {
        // Pass model to QML context
        m_context->setContextProperty("outParametersModel", outParametersModel);
        //qDebug() << "QmlMainWindow: Out parameters model passed to QML";
    }
	else 
    {
        //qDebug() << "QmlMainWindow: Out parameters model not found, will use default data";
        // Create empty model to avoid errors
        m_context->setContextProperty("outParametersModel", nullptr);
    }
    
    // Добавляем объект QmlMainWindow в QML контекст для вызова методов
    m_context->setContextProperty("qmlMainWindow", this);
}

void QmlMainWindow::setupChartSeriesModel()
{
    if (!m_app || !m_context) return;
    
    // Create charts list model
    auto chartsListModel = new ChartsListModel(this);
    chartsListModel->setParametersStorage(m_app->getParametersStorage());

    // auto speedSeries = chartsListModel->addChart("Speed");
    // auto altitudeSeries = chartsListModel->addChart("Altitude");

    //seriesModel->addPoint("speed", 0, 0);
    //seriesModel->addPoint("speed", 1, 1);

    // Add some default charts
    //chartsListModel->addChart("Основной график", QStringList() << "Altitude" << "Speed");
    //chartsListModel->addChart("Координаты", QStringList() << "Latitude" << "Longitude");
    
    // Pass model to QML context as chartSeriesModel (for compatibility with existing QML)
    m_context->setContextProperty("chartsListModel", chartsListModel);
}

void QmlMainWindow::setupConnections()
{
}

void QmlMainWindow::onParameterUpdated(const QString &label)
{
    // Update QML interface when parameter changes
    //qDebug() << "QmlMainWindow: Parameter updated:" << label;
    
    // Can send signal to QML to update interface
    if (m_context) 
    {

        auto boardParametersStorage = getApp()->getParametersStorage();

        auto boardParameter = boardParametersStorage->getParameterHistory(label);
        Q_UNUSED(boardParameter); // Подавляем предупреждение о неиспользуемой переменной
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
