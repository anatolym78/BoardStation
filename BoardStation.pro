QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += charts
QT += quick

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BoardStationApp.cpp \
    Model/Emulation/BoardDataEmulator.cpp \
    Model/Emulation/BoardDataGenerators/ParameterGenerator.cpp \
    Model/Emulation/BoardDataGenerators/AltitudeGenerator.cpp \
    Model/Emulation/BoardDataGenerators/LongitudeGenerator.cpp \
    Model/Emulation/BoardDataGenerators/LatitudeGenerator.cpp \
    Model/Emulation/BoardDataGenerators/SpeedGenerator.cpp \
    Model/JsonReader.cpp \
    Model/JsonWriter.cpp \
    Model/ParametersStorage.cpp \
    Interface/Charts/ChartBuilder.cpp \
    ViewModel/ParametersListModel.cpp \
    ViewModel/OutParametersModel.cpp \
    ViewModel/Parameters/OutParameter.cpp \
    ViewModel/Parameters/StringOutParameter.cpp \
    ViewModel/Parameters/RealOutParameter.cpp \
    ViewModel/Parameters/RangedRealOutParameter.cpp \
    ViewModel/Parameters/ListedRealOutParameter.cpp \
    ViewModel/Parameters/BooleanOutParameter.cpp \
    ViewModel/Parameters/OutParametersStorage.cpp \
    ViewModel/Parameters/ConfigurationReader.cpp \
    ViewModel/Parameters/ParametersParser.cpp \
    ViewModel/Parameters/ListedRealOutParameterDelegate.cpp \
    ViewModel/Parameters/OutParameterWidgetCreator.cpp \
    main.cpp \
    mainwindow.cpp \
    QmlMainWindow.cpp

HEADERS += \
    BoardStationApp.h \
    Model/Emulation/BoardDataEmulator.h \
    Model/Emulation/BoardDataGenerators/ParameterGenerator.h \
    Model/Emulation/BoardDataGenerators/AltitudeGenerator.h \
    Model/Emulation/BoardDataGenerators/LongitudeGenerator.h \
    Model/Emulation/BoardDataGenerators/LatitudeGenerator.h \
    Model/Emulation/BoardDataGenerators/SpeedGenerator.h \
    Model/IDriver.h \
    Model/JsonReader.h \
    Model/JsonWriter.h \
    Model/Parameter.h \
    Model/ParametersStorage.h \
    Interface/Charts/ChartBuilder.h \
    ViewModel/ParametersListModel.h \
    ViewModel/OutParametersModel.h \
    ViewModel/Parameters/OutParameter.h \
    ViewModel/Parameters/StringOutParameter.h \
    ViewModel/Parameters/RealOutParameter.h \
    ViewModel/Parameters/RangedRealOutParameter.h \
    ViewModel/Parameters/ListedRealOutParameter.h \
    ViewModel/Parameters/BooleanOutParameter.h \
    ViewModel/Parameters/OutParametersStorage.h \
    ViewModel/Parameters/ConfigurationReader.h \
    ViewModel/Parameters/ParametersParser.h \
    ViewModel/Parameters/ListedRealOutParameterDelegate.h \
    ViewModel/Parameters/Parameters.h \
    ViewModel/Parameters/OutParameterWidgetCreator.h \
    mainwindow.h \
    QmlMainWindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    qml.qrc

TRANSLATIONS += \
    BoardStation_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
