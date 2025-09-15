#ifndef BOARDMESSAGESJSONWRITERNEW_H
#define BOARDMESSAGESJSONWRITERNEW_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>
#include "BoardParameterSingle.h"

class BoardMessagesJsonWriterNew : public QObject
{
    Q_OBJECT

public:
    explicit BoardMessagesJsonWriterNew(const QString &filename = "CaptureBoardData.json", 
                                       QObject *parent = nullptr);
    ~BoardMessagesJsonWriterNew();
    
    // Добавление сообщения в очередь - теперь принимает отдельные параметры
    void addMessage(const QList<BoardParameterSingle*> &parameters, 
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
    // Структура для хранения сообщения
    struct ParameterMessage 
    {
        QList<BoardParameterSingle*> parameters;
        QDateTime timestamp;
    };

    // Чтение существующего файла
    QJsonObject readExistingFile();
    
    // Запись в файл
    bool writeToFile(const QJsonObject &data);
    
    // Создание имени блока
    QString generateBlockName(int blockNumber) const;
    
    // Преобразование параметров в JSON объект
    QJsonObject parametersToJsonObject(const QList<BoardParameterSingle*> &parameters, const QDateTime &timestamp) const;

private:
    QString m_filename;
    QTimer *m_writeTimer;
    QQueue<ParameterMessage*> m_messageQueue;
    int m_writeInterval;
    int m_blockCounter;
    QMutex m_queueMutex;
};

#endif // BOARDMESSAGESJSONWRITERNEW_H
