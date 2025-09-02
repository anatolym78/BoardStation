#ifndef BOARDMESSAGESJSONWRITER_H
#define BOARDMESSAGESJSONWRITER_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>
#include "BoardMessage.h"

class BoardMessagesJsonWriter : public QObject
{
    Q_OBJECT

public:
    explicit BoardMessagesJsonWriter(const QString &filename = "CaptureBoardData.json", 
                                    QObject *parent = nullptr);
    ~BoardMessagesJsonWriter();
    
    // Добавление сообщения в очередь
    void addMessage(const QList<BoardParameter> &parameters, 
                   const QDateTime &timestamp = QDateTime::currentDateTime());
    
    // Очистка файла (вызывается при запуске приложения)
    void clearFile();
    
    // Принудительная запись всех сообщений из очереди
    void flushQueue();
    
    // Установка интервала записи (в миллисекундах)
    void setWriteInterval(int intervalMs);
    
    // Получение текущего интервала записи
    int getWriteInterval() const { return m_writeInterval; }

signals:
    void writeError(const QString &error);
    void writeSuccess(const QString &message);
    void fileCleared();

private slots:
    void writeMessagesToFile();

private:
    // Чтение существующего файла
    QJsonObject readExistingFile();
    
    // Запись в файл
    bool writeToFile(const QJsonObject &data);
    
    // Создание имени блока
    QString generateBlockName(int blockNumber) const;

private:
    QString m_filename;
    QTimer *m_writeTimer;
    QQueue<BoardMessage*> m_messageQueue;
    int m_writeInterval;
    int m_blockCounter;
    QMutex m_queueMutex;
};

#endif // BOARDMESSAGESJSONWRITER_H
