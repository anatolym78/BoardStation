#include "BoardMessagesJsonWriterNew.h"
#include <QFile>
#include <QTextStream>
#include <QJsonParseError>
#include <QDebug>
#include <QMutexLocker>
#include <QApplication>

BoardMessagesJsonWriterNew::BoardMessagesJsonWriterNew(const QString &filename, QObject *parent)
    : QObject(parent)
    , m_filename(QApplication::applicationDirPath() + "/" + filename)
    , m_writeInterval(1000) // 1 секунда по умолчанию
    , m_blockCounter(1)
{
    m_writeTimer = new QTimer(this);
    connect(m_writeTimer, &QTimer::timeout, this, &BoardMessagesJsonWriterNew::writeMessagesToFile);
    m_writeTimer->start(m_writeInterval);
    
    //qDebug() << "BoardMessagesJsonWriterNew: Initialized with file" << m_filename;
    //qDebug() << "BoardMessagesJsonWriterNew: Application directory:" << QApplication::applicationDirPath();
}

BoardMessagesJsonWriterNew::~BoardMessagesJsonWriterNew()
{
    // Записываем оставшиеся сообщения перед уничтожением
    flushQueue();
    
    if (m_writeTimer) 
    {
        m_writeTimer->stop();
    }
    
    // Очищаем очередь
    QMutexLocker locker(&m_queueMutex);
    while (!m_messageQueue.isEmpty()) 
    {
        ParameterMessage *msg = m_messageQueue.dequeue();
        qDeleteAll(msg->parameters);
        delete msg;
    }
}

void BoardMessagesJsonWriterNew::addMessage(const QList<BoardParameterSingle*> &parameters, 
                                           const QDateTime &timestamp)
{
    if (parameters.isEmpty()) 
    {
        //qDebug() << "BoardMessagesJsonWriterNew: Skipping empty message";
        return;
    }
    
    QMutexLocker locker(&m_queueMutex);
    
    // Создаём копии параметров для безопасного хранения
    ParameterMessage *message = new ParameterMessage();
    message->timestamp = timestamp;
    
    for (BoardParameterSingle *param : parameters) 
    {
        if (param) 
        {
            // Создаём копию параметра
            BoardParameterSingle *paramCopy = new BoardParameterSingle(
                param->label(), 
                param->value(), 
                param->timestamp(), 
                param->unit(), 
                this
            );
            message->parameters.append(paramCopy);
        }
    }
    
    m_messageQueue.enqueue(message);
    m_blockCounter++;
    
    //qDebug() << "BoardMessagesJsonWriterNew: Added message with" << message->parameters.size() << "parameters";
}

void BoardMessagesJsonWriterNew::clearFile()
{
    QFile file(m_filename);
    if (file.exists()) 
    {
        if (file.remove())
        {
            //qDebug() << "BoardMessagesJsonWriterNew: File cleared successfully";
            emit fileCleared();
        }
        else
        {
            qWarning() << "BoardMessagesJsonWriterNew: Failed to clear file";
            emit writeError("Не удалось очистить файл");
        }
    }
    else
    {
        //qDebug() << "BoardMessagesJsonWriterNew: File does not exist, nothing to clear";
    }

    m_blockCounter = 1;
}

void BoardMessagesJsonWriterNew::flushQueue()
{
    writeMessagesToFile();
}

void BoardMessagesJsonWriterNew::setWriteInterval(int intervalMs)
{
    m_writeInterval = intervalMs;
    if (m_writeTimer)
    {
        m_writeTimer->setInterval(intervalMs);
    }
}

void BoardMessagesJsonWriterNew::writeMessagesToFile()
{
    QMutexLocker locker(&m_queueMutex);
    
    if (m_messageQueue.isEmpty())
    {
        return;
    }
    
    int messageCount = m_messageQueue.size();
    //qDebug() << "BoardMessagesJsonWriterNew: Writing" << messageCount << "messages to file";
    
    // Читаем существующий файл
    QJsonObject existingData = readExistingFile();
    
    // Добавляем новые сообщения
    while (!m_messageQueue.isEmpty())
    {
        ParameterMessage *message = m_messageQueue.dequeue();
        QString blockName = generateBlockName(m_blockCounter);
        existingData[blockName] = parametersToJsonObject(message->parameters, message->timestamp);
        
        // Удаляем сообщение после использования
        qDeleteAll(message->parameters);
        delete message;
    }
    
    // Записываем обновленные данные
    if (writeToFile(existingData)) 
    {
        emit writeSuccess(QString("Записано %1 сообщений в блок").arg(messageCount));
    } 
    else 
    {
        emit writeError("Ошибка записи в файл");
    }
}

QJsonObject BoardMessagesJsonWriterNew::readExistingFile()
{
    QFile file(m_filename);
    if (!file.exists()) 
    {
        return QJsonObject();
    }
    
    if (!file.open(QIODevice::ReadOnly)) 
    {
        qWarning() << "BoardMessagesJsonWriterNew: Cannot open file for reading:" << m_filename;
        return QJsonObject();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    // Проверяем, что данные не пустые
    if (data.isEmpty()) 
    {
        //qDebug() << "BoardMessagesJsonWriterNew: File is empty";
        return QJsonObject();
    }
    
    // Пытаемся исправить проблемы с кодировкой
    QString jsonString = QString::fromUtf8(data);
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) 
    {
        qWarning() << "BoardMessagesJsonWriterNew: JSON parse error:" << error.errorString();
        qWarning() << "BoardMessagesJsonWriterNew: JSON content:" << jsonString.left(200);
        return QJsonObject();
    }
    
    if (doc.isObject()) 
    {
        return doc.object();
    }
    
    return QJsonObject();
}

bool BoardMessagesJsonWriterNew::writeToFile(const QJsonObject &data)
{
    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly)) 
    {
        qWarning() << "BoardMessagesJsonWriterNew: Cannot open file for writing:" << m_filename;
        return false;
    }
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    // Записываем данные с явным указанием кодировки UTF-8
    qint64 bytesWritten = file.write(jsonData);
    file.close();
    
    if (bytesWritten != jsonData.size()) 
    {
        qWarning() << "BoardMessagesJsonWriterNew: Failed to write all data to file";
        return false;
    }
    
    return true;
}

QString BoardMessagesJsonWriterNew::generateBlockName(int blockNumber) const
{
    return QString("Message " + QString::number(blockNumber));
}

QJsonObject BoardMessagesJsonWriterNew::parametersToJsonObject(const QList<BoardParameterSingle*> &parameters, const QDateTime &timestamp) const
{
    QJsonObject messageObj;
    messageObj["timestamp"] = timestamp.toString(Qt::ISODate);
    
    QJsonArray parametersArray;
    for (const BoardParameterSingle *param : parameters) 
    {
        if (param) 
        {
            QJsonObject paramObj;
            paramObj["label"] = param->label();
            paramObj["value"] = QJsonValue::fromVariant(param->value());
            paramObj["unit"] = param->unit();
            paramObj["timestamp"] = param->timestamp().toString(Qt::ISODate);
            
            parametersArray.append(paramObj);
        }
    }
    
    messageObj["parameters"] = parametersArray;
    return messageObj;
}
