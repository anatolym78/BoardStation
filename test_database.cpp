/*
 * Тестовый скрипт для проверки создания базы данных
 * 
 * Этот файл можно использовать для тестирования создания и сохранения базы данных
 * независимо от основного приложения.
 */

#include "Model/Parameters/BoardMessagesSqliteWriter.h"
#include "Model/Parameters/BoardMessagesSqliteReader.h"
#include "Model/Parameters/BoardParameterSingle.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "=== Тест создания базы данных SQLite ===";
    qDebug() << "Папка приложения:" << QApplication::applicationDirPath();
    
    // Создаем writer
    BoardMessagesSqliteWriter writer("TestBoardStationData.db");
    
    qDebug() << "Путь к базе данных:" << writer.getDatabasePath();
    
    // Проверяем, что файл создался
    QFileInfo dbFile(writer.getDatabasePath());
    qDebug() << "Файл существует:" << dbFile.exists();
    qDebug() << "Размер файла:" << dbFile.size() << "байт";
    
    // Создаем тестовую сессию
    writer.createNewSession("Тестовая сессия");
    
    // Создаем тестовые параметры
    QList<BoardParameterSingle*> testParams;
    testParams.append(new BoardParameterSingle("Altitude", 100.5, QDateTime::currentDateTime(), "m"));
    testParams.append(new BoardParameterSingle("Speed", 25.3, QDateTime::currentDateTime(), "km/h"));
    testParams.append(new BoardParameterSingle("Temperature", 22.1, QDateTime::currentDateTime(), "°C"));
    
    // Добавляем параметры в базу
    writer.addMessage(testParams);
    
    // Принудительно сохраняем
    writer.forceSave();
    
    // Проверяем размер файла после записи
    qDebug() << "Размер файла после записи:" << dbFile.size() << "байт";
    
    // Создаем reader и проверяем данные
    BoardMessagesSqliteReader reader("TestBoardStationData.db");
    auto sessions = reader.getAvailableSessions();
    
    qDebug() << "Найдено сессий:" << sessions.size();
    for (const auto &session : sessions) 
    {
        qDebug() << "Сессия:" << session.name << "ID:" << session.id << "Сообщений:" << session.messageCount;
    }
    
    // Принудительно сохраняем перед закрытием
    writer.forceSave();
    
    qDebug() << "=== Тест завершен ===";
    qDebug() << "Финальный размер файла:" << dbFile.size() << "байт";
    qDebug() << "Файл существует:" << dbFile.exists();
    
    return 0;
}
