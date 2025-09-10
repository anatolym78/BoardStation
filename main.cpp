#include "BoardStationApp.h"

// Макрос для переключения между интерфейсами
#define QML_GUI

#ifdef QML_GUI
    #include "QmlMainWindow.h"
#else
    #include "mainwindow.h"
#endif

#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    BoardStationApp app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BoardStation_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    
#ifdef QML_GUI
    QmlMainWindow w;
    w.setApp(&app);
    w.show();
#else
    MainWindow w;
    w.setApp(&app);
    w.showMaximized();
#endif
    
    return app.exec();
}
