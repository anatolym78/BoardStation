QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += charts
QT += quick
QT += sql
QT += quickcontrols2
#QT += quick_templates2

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BoardStationApp.cpp \
    Interface/ChartsPanel.cpp \
    Interface/ParametersChartView.cpp \
    Interface/ParametersTreeView.cpp \
    Interface/PlayerView.cpp \
    Interface/SessionFrame.cpp \
    Interface/SessionListView.cpp \
    Interface/SessionsStackWidget.cpp \
    Model/Emulation/BoardDataEmulator.cpp \
    Model/Emulation/BoardDataGenerators/AccelerationGenerator.cpp \
    Model/Emulation/BoardDataGenerators/BatteryVoltageGenerator.cpp \
    Model/Emulation/BoardDataGenerators/DesiredAngleRatesGenerator.cpp \
    Model/Emulation/BoardDataGenerators/GpsLocationGenerator.cpp \
    Model/Emulation/BoardDataGenerators/GroundSpeedGenerator.cpp \
    Model/Emulation/BoardDataGenerators/GyroAngleRatesGenerator.cpp \
    Model/Emulation/BoardDataGenerators/IsArmedGenerator.cpp \
    Model/Emulation/BoardDataGenerators/ParameterGenerator.cpp \
    Model/Emulation/BoardDataGenerators/PositionAltitudeGenerator.cpp \
    Model/Emulation/BoardDataGenerators/ThrottleAdjustmentValuesGenerator.cpp \
    Model/Emulation/BoardDataGenerators/BoardDataJsonGenerator.cpp \
    Model/Parameters/BoardParameterHistory.cpp \
    Model/Parameters/BoardParametersJsonParser.cpp \
    Model/Parameters/BoardParametersJsonParserNew.cpp \
    Model/Parameters/BoardParameterValue.cpp \
    Model/Parameters/BoardParameterSingle.cpp \
    Model/Parameters/BoardParametersStorage.cpp \
    Model/Parameters/BoardParameterHistoryStorage.cpp \
    Model/Parameters/ParameterTreeJsonParser.cpp \
    Model/Parameters/Tree/ParameterTreeGroupItem.cpp \
    Model/Parameters/Tree/ParameterTreeHistoryItem.cpp \
    Model/Parameters/Tree/ParameterTreeArrayItem.cpp \
    Model/Parameters/Tree/ParameterTreeItem.cpp \
    Model/Parameters/Tree/ParameterTreeStorage.cpp \
    ViewModel/BoardParameterTreeItem.cpp \
    ViewModel/BoardParametersListModel.cpp \
    ViewModel/BoardParametersTreeModel.cpp \
    ViewModel/ChartViewModel.cpp \
    ViewModel/ChatViewGridModel.cpp \
    ViewModel/UplinkParametersModel.cpp \
    ViewModel/DebugViewModel.cpp \
    ViewModel/SessionsListModel.cpp \
    ViewModel/Session.cpp \
    ViewModel/RecordedSession.cpp \
    ViewModel/LiveSession.cpp \
    ViewModel/RecordedSessionsFactory.cpp \
    ViewModel/LiveSessionFactory.cpp \
    ViewModel/SessionPlayer.cpp \
    ViewModel/DataPlayer.cpp \
    ViewModel/DriverDataPlayer.cpp \
    ViewModel/SimpleTreeItem.cpp \
    ViewModel/SimpleTreeModel.cpp \
    Model/Parameters/BasicUplinkParameter.cpp \
    Model/Parameters/IntUplinkParameter.cpp \
    Model/Parameters/DoubleUplinkParameter.cpp \
    Model/Parameters/StringUplinkParameter.cpp \
    Model/Parameters/BoolUplinkParameter.cpp \
    Model/Parameters/UplinkParametersParser.cpp \
    Model/Parameters/AppConfigurationReader.cpp \
    Model/Parameters/BoardMessage.cpp \
    Model/Parameters/BoardMessagesJsonWriter.cpp \
    Model/Parameters/BoardMessagesJsonWriterNew.cpp \
    Model/Parameters/BoardMessagesSqliteWriter.cpp \
    Model/Parameters/BoardMessagesSqliteReader.cpp \
    Model/DriverAdapter.cpp \
    main.cpp \
    QmlMainWindow.cpp \
    mainwindow.cpp

HEADERS += \
    BoardStationApp.h \
    Interface/ChartsPanel.h \
    Interface/ParametersChartView.h \
    Interface/ParametersTreeView.h \
    Interface/PlayerView.h \
    Interface/SessionFrame.h \
    Interface/SessionListView.h \
    Interface/SessionsStackWidget.h \
    Model/Emulation/BoardDataEmulator.h \
    Model/Emulation/BoardDataGenerators/AccelerationGenerator.h \
    Model/Emulation/BoardDataGenerators/BatteryVoltageGenerator.h \
    Model/Emulation/BoardDataGenerators/DesiredAngleRatesGenerator.h \
    Model/Emulation/BoardDataGenerators/GpsLocationGenerator.h \
    Model/Emulation/BoardDataGenerators/GroundSpeedGenerator.h \
    Model/Emulation/BoardDataGenerators/GyroAngleRatesGenerator.h \
    Model/Emulation/BoardDataGenerators/IsArmedGenerator.h \
    Model/Emulation/BoardDataGenerators/ParameterGenerator.h \
    Model/Emulation/BoardDataGenerators/PositionAltitudeGenerator.h \
    Model/Emulation/BoardDataGenerators/ThrottleAdjustmentValuesGenerator.h \
    Model/Emulation/BoardDataGenerators/BoardDataJsonGenerator.h \
    Model/IDriver.h \
    Model/Parameters/BoardParametersJsonParser.h \
    Model/Parameters/BoardParametersJsonParserNew.h \
    Model/Parameters/BoardParameterSingle.h \
    Model/Parameters/BoardParameterValue.h \
    Model/Parameters/BoardParametersStorage.h \
    Model/Parameters/BoardParameterHistory.h \
    Model/Parameters/BoardParameterHistoryStorage.h \
    Model/Parameters/ParameterTreeJsonParser.h \
    Model/Parameters/Tree/ParameterTreeGroupItem.h \
    Model/Parameters/Tree/ParameterTreeHistoryItem.h \
    Model/Parameters/Tree/ParameterTreeArrayItem.h \
    Model/Parameters/Tree/ParameterTreeItem.h \
    Model/Parameters/Tree/ParameterTreeStorage.h \
    ViewModel/BoardParameterTreeItem.h \
    ViewModel/BoardParametersListModel.h \
    ViewModel/BoardParametersTreeModel.h \
    ViewModel/ChartViewModel.h \
    ViewModel/ChatViewGridModel.h \
    ViewModel/UplinkParametersModel.h \
    ViewModel/DebugViewModel.h \
    ViewModel/SessionsListModel.h \
    ViewModel/Session.h \
    ViewModel/RecordedSession.h \
    ViewModel/LiveSession.h \
    ViewModel/RecordedSessionsFactory.h \
    ViewModel/LiveSessionFactory.h \
    ViewModel/SessionPlayer.h \
    ViewModel/DataPlayer.h \
    ViewModel/DriverDataPlayer.h \
    ViewModel/SimpleTreeItem.h \
    ViewModel/SimpleTreeModel.h \
    Model/Parameters/BasicUplinkParameter.h \
    Model/Parameters/TypedUplinkParameter.h \
    Model/Parameters/Range.h \
    Model/Parameters/Step.h \
    Model/Parameters/IntUplinkParameter.h \
    Model/Parameters/DoubleUplinkParameter.h \
    Model/Parameters/StringUplinkParameter.h \
    Model/Parameters/BoolUplinkParameter.h \
    Model/Parameters/UplinkParametersParser.h \
    Model/Parameters/AppConfigurationReader.h \
    Model/Parameters/BoardMessage.h \
    Model/Parameters/BoardMessagesJsonWriter.h \
    Model/Parameters/BoardMessagesJsonWriterNew.h \
    Model/Parameters/BoardMessagesSqliteWriter.h \
    Model/Parameters/BoardMessagesSqliteReader.h \
    Model/DriverAdapter.h \
    Model/Parameters/Parameters.h \
    QmlMainWindow.h \
    mainwindow.h

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

DISTFILES +=
