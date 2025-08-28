#include "mainwindow.h"
#include "BoardStationApp.h"

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
    
    MainWindow w;
    w.setApp(&app);
    w.showMaximized();
    
    return app.exec();
}
