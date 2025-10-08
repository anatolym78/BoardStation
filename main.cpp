#include "BoardStationApp.h"

// Макрос для переключения между интерфейсами

#include "QmlMainWindow.h"

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
    
    QmlMainWindow w;
    w.setApp(&app);
    w.show();

    return app.exec();
}
