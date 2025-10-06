QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += charts
QT += quick
QT += sql

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
    Model/Emulation/BoardDataGenerators/BoardDataJsonGenerator.cpp \
    Model/Parameters/BoardParametersJsonParser.cpp \
    Model/Parameters/BoardParametersJsonParserNew.cpp \
    Model/Parameters/BoardParameterValue.cpp \
    Model/Parameters/BoardParameterSingle.cpp \
    Model/Parameters/BoardParameter.cpp \
    Model/Parameters/BoardParameterHistory.cpp \
    Model/Parameters/BoardParametersStorage.cpp \
    Model/Parameters/BoardParameterHistoryStorage.cpp \
    Interface/Charts/ChartBuilder.cpp \
    ViewModel/BoardParametersListModel.cpp \
    ViewModel/BoardParametersModel.cpp \
    ViewModel/ChartPointsModel.cpp \
    ViewModel/ChartSeriesModel.cpp \
    ViewModel/ChartsListModel.cpp \
    ViewModel/ChartViewModel.cpp \
    ViewModel/OutParametersModel.cpp \
    ViewModel/UplinkParametersModel.cpp \
    ViewModel/DebugViewModel.cpp \
    ViewModel/SessionsListModel.cpp \
    ViewModel/SessionPlayer.cpp \
    ViewModel/DataPlayer.cpp \
    ViewModel/DriverDataPlayer.cpp \
    Model/Parameters/OutParameter.cpp \
    Model/Parameters/StringOutParameter.cpp \
    Model/Parameters/RealOutParameter.cpp \
    Model/Parameters/RangedRealOutParameter.cpp \
    Model/Parameters/ListedRealOutParameter.cpp \
    Model/Parameters/BooleanOutParameter.cpp \
    Model/Parameters/BasicUplinkParameter.cpp \
    Model/Parameters/IntUplinkParameter.cpp \
    Model/Parameters/DoubleUplinkParameter.cpp \
    Model/Parameters/StringUplinkParameter.cpp \
    Model/Parameters/BoolUplinkParameter.cpp \
    Model/Parameters/UplinkParametersParser.cpp \
    Model/Parameters/OutParametersStorage.cpp \
    Model/Parameters/AppConfigurationReader.cpp \
    Model/Parameters/OutParametersParser.cpp \
    Model/Parameters/BoardMessage.cpp \
    Model/Parameters/BoardMessagesJsonWriter.cpp \
    Model/Parameters/BoardMessagesJsonWriterNew.cpp \
    Model/Parameters/BoardMessagesSqliteWriter.cpp \
    Model/Parameters/BoardMessagesSqliteReader.cpp \
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
    Model/Emulation/BoardDataGenerators/BoardDataJsonGenerator.h \
    Model/IDriver.h \
    Model/Parameters/BoardParametersJsonParser.h \
    Model/Parameters/BoardParametersJsonParserNew.h \
    Model/Parameters/BoardParameter.h \
    Model/Parameters/BoardParameterSingle.h \
    Model/Parameters/BoardParameterHistory.h \
    Model/Parameters/BoardParameterValue.h \
    Model/Parameters/BoardParametersStorage.h \
    Model/Parameters/BoardParameterHistoryStorage.h \
    Interface/Charts/ChartBuilder.h \
    ViewModel/BoardParametersListModel.h \
    ViewModel/BoardParametersModel.h \
    ViewModel/ChartPointsModel.h \
    ViewModel/ChartSeriesModel.h \
    ViewModel/ChartsListModel.h \
    ViewModel/ChartViewModel.h \
    ViewModel/OutParametersModel.h \
    ViewModel/UplinkParametersModel.h \
    ViewModel/DebugViewModel.h \
    ViewModel/SessionsListModel.h \
    ViewModel/SessionPlayer.h \
    ViewModel/DataPlayer.h \
    ViewModel/DriverDataPlayer.h \
    Model/Parameters/OutParameter.h \
    Model/Parameters/StringOutParameter.h \
    Model/Parameters/RealOutParameter.h \
    Model/Parameters/RangedRealOutParameter.h \
    Model/Parameters/ListedRealOutParameter.h \
    Model/Parameters/BooleanOutParameter.h \
    Model/Parameters/BasicUplinkParameter.h \
    Model/Parameters/TypedUplinkParameter.h \
    Model/Parameters/Range.h \
    Model/Parameters/Step.h \
    Model/Parameters/IntUplinkParameter.h \
    Model/Parameters/DoubleUplinkParameter.h \
    Model/Parameters/StringUplinkParameter.h \
    Model/Parameters/BoolUplinkParameter.h \
    Model/Parameters/UplinkParametersParser.h \
    Model/Parameters/OutParametersStorage.h \
    Model/Parameters/AppConfigurationReader.h \
    Model/Parameters/OutParametersParser.h \
    Model/Parameters/BoardMessage.h \
    Model/Parameters/BoardMessagesJsonWriter.h \
    Model/Parameters/BoardMessagesJsonWriterNew.h \
    Model/Parameters/BoardMessagesSqliteWriter.h \
    Model/Parameters/BoardMessagesSqliteReader.h \
    Model/Parameters/Parameters.h \
    mainwindow.h \
    QmlMainWindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    qml.qrc

# TRANSLATIONS += \
#     BoardStation_ru_RU.ts
CONFIG -= lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
