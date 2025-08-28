QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BoardStationApp.cpp \
    Model/Emulation/BoardDataEmulator.cpp \
    Model/Emulation/ParameterGenerator.cpp \
    Model/Emulation/AltitudeGenerator.cpp \
    Model/Emulation/LongitudeGenerator.cpp \
    Model/Emulation/LatitudeGenerator.cpp \
    Model/Emulation/SpeedGenerator.cpp \
    Model/JsonReader.cpp \
    Model/JsonWriter.cpp \
    Model/ParametersStorage.cpp \
    Model/ChartBuilder.cpp \
    ViewModel/ParametersListModel.cpp \
    ViewModel/OutParametersModel.cpp \
    ViewModel/Parameters/OutParameter.cpp \
    ViewModel/Parameters/StringOutParameter.cpp \
    ViewModel/Parameters/RealOutParameter.cpp \
    ViewModel/Parameters/RangedRealOutParameter.cpp \
    ViewModel/Parameters/ListedRealOutParameter.cpp \
    ViewModel/Parameters/ConfigurationReader.cpp \
    ViewModel/Parameters/ParametersParser.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    BoardStationApp.h \
    Model/Emulation/BoardDataEmulator.h \
    Model/Emulation/ParameterGenerator.h \
    Model/Emulation/AltitudeGenerator.h \
    Model/Emulation/LongitudeGenerator.h \
    Model/Emulation/LatitudeGenerator.h \
    Model/Emulation/SpeedGenerator.h \
    Model/IDriver.h \
    Model/JsonReader.h \
    Model/JsonWriter.h \
    Model/Parameter.h \
    Model/ParametersStorage.h \
    Model/ChartBuilder.h \
    ViewModel/ParametersListModel.h \
    ViewModel/OutParametersModel.h \
    ViewModel/Parameters/OutParameter.h \
    ViewModel/Parameters/StringOutParameter.h \
    ViewModel/Parameters/RealOutParameter.h \
    ViewModel/Parameters/RangedRealOutParameter.h \
    ViewModel/Parameters/ListedRealOutParameter.h \
    ViewModel/Parameters/ConfigurationReader.h \
    ViewModel/Parameters/ParametersParser.h \
    ViewModel/Parameters/Parameters.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    BoardStation_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
