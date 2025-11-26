cmake_minimum_required(VERSION 3.16)

project(RCDriver LANGUAGES CXX)


find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost 1.83 REQUIRED)

# Собираем исходники для библиотеки (все, кроме main.cpp)
set(LIB_SOURCES
    driverfactory.cpp
    serialdriver.cpp
    messagepackage.cpp
    common/crc8.cpp
    driver/protocolfactory.cpp
    driver/serialprotocol.cpp
    io/context.cpp
    io/devicefactory.cpp
    io/message.cpp
    io/serialport.cpp
    listeners/messagelistener.cpp
    listeners/notifier.cpp
    logic/devicemanager.cpp
)

set(LIB_HEADERS
    driverinterface.h
    driverfactory.h
    datastructures.h
    serialdriver.h
    messagepackage.h
    common/crc8.h
    driver/protocolfactory.h
    driver/serialprotocol.h
    io/context.h
    io/devicefactory.h
    io/message.h
    io/serialport.h
    io/iodevice.h
    io/serial.h
    io/connection.h
    listeners/messagelistener.h
    listeners/notifier.h
    listeners/connectionlistener.h
    listeners/listener.h
    listeners/datalistener.h
    listeners/listenermanager.h
    listeners/subject.h
    listeners/subjectcontainer.h
    logic/devicemanager.h
    logic/manager.h
)

# Создаём статическую библиотеку
add_library(RCDriver STATIC ${LIB_SOURCES} ${LIB_HEADERS})

# Включаем заголовочные файлы для библиотеки
target_include_directories(RCDriver PUBLIC .)
target_include_directories(RCDriver PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(RCDriver
    Qt5::Core
    Qt5::Concurrent
    ${Boost_LIBRARIES}
)
