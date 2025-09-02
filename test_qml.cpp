#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    
    // Загружаем QML файл
    engine.load(QUrl("qrc:/main_simple.qml"));
    
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML file";
        return -1;
    }
    
    qDebug() << "QML file loaded successfully";
    
    return app.exec();
}
