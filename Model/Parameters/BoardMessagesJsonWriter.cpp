#include "BoardMessagesJsonWriter.h"
#include <QFile>
#include <QTextStream>
#include <QJsonParseError>
#include <QDebug>
#include <QMutexLocker>
#include <QApplication>

BoardMessagesJsonWriter::BoardMessagesJsonWriter(const QString &filename, QObject *parent)
    : QObject(parent)
    , m_filename(QApplication::applicationDirPath() + "/" + filename)
    , m_writeInterval(1000) // 1 секунда по умолчанию
    , m_blockCounter(1)
{
    m_writeTimer = new QTimer(this);
    connect(m_writeTimer, &QTimer::timeout, this, &BoardMessagesJsonWriter::writeMessagesToFile);
    m_writeTimer->start(m_writeInterval);
    
    qDebug() << "BoardMessagesJsonWriter: Initialized with file" << m_filename;
    qDebug() << "BoardMessagesJsonWriter: Application directory:" << QApplication::applicationDirPath();
}

BoardMessagesJsonWriter::~BoardMessagesJsonWriter()
{
    // Записываем оставшиеся сообщения перед уничтожением
    flushQueue();
    
    if (m_writeTimer) {
        m_writeTimer->stop();
    }
}

void BoardMessagesJsonWriter::addMessage(const QList<BoardParameter*> &parameters, 
                                         const QDateTime &timestamp)
{
    if (parameters.isEmpty()) {
        qDebug() << "BoardMessagesJsonWriter: Skipping empty message";
        return;
    }
    
    QMutexLocker locker(&m_queueMutex);
    BoardMessage *message = new BoardMessage(parameters, timestamp, this);
    m_messageQueue.enqueue(message);

    m_blockCounter++;
    
    //qDebug() << "BoardMessagesJsonWriter: Added message with" << parameters.size() << "parameters";
}

void BoardMessagesJsonWriter::clearFile()
{
    QFile file(m_filename);
    if (file.exists()) 
    {
        if (file.remove())
        {
            qDebug() << "BoardMessagesJsonWriter: File cleared successfully";
            emit fileCleared();
        }
    	else
        {
            qWarning() << "BoardMessagesJsonWriter: Failed to clear file";
            emit writeError("Не удалось очистить файл");
        }
    }
	else
    {
        qDebug() << "BoardMessagesJsonWriter: File does not exist, nothing to clear";
    }

    m_blockCounter = 1;
}

void BoardMessagesJsonWriter::flushQueue()
{
    writeMessagesToFile();
}

void BoardMessagesJsonWriter::setWriteInterval(int intervalMs)
{
    m_writeInterval = intervalMs;
    if (m_writeTimer)
    {
        m_writeTimer->setInterval(intervalMs);
    }
}

void BoardMessagesJsonWriter::writeMessagesToFile()
{
    QMutexLocker locker(&m_queueMutex);
    
    if (m_messageQueue.isEmpty())
    {
        return;
    }
    
    int messageCount = m_messageQueue.size();
    //qDebug() << "BoardMessagesJsonWriter: Writing" << messageCount << "messages to file";
    
    // Читаем существующий файл
    QJsonObject existingData = readExistingFile();
    
    // Добавляем новые сообщения
    while (!m_messageQueue.isEmpty())
    {
        BoardMessage *message = m_messageQueue.dequeue();
        QString blockName = generateBlockName(m_blockCounter);
        existingData[blockName] = message->toJsonObject();
        
        // Удаляем сообщение после использования
        message->deleteLater();
    }
    
    // Записываем обновленные данные
    if (writeToFile(existingData)) {
        emit writeSuccess(QString("Записано %1 сообщений в блок").arg(messageCount));
    } else {
        emit writeError("Ошибка записи в файл");
    }
}

QJsonObject BoardMessagesJsonWriter::readExistingFile()
{
    QFile file(m_filename);
    if (!file.exists()) {
        return QJsonObject();
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "BoardMessagesJsonWriter: Cannot open file for reading:" << m_filename;
        return QJsonObject();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    // Проверяем, что данные не пустые
    if (data.isEmpty()) {
        qDebug() << "BoardMessagesJsonWriter: File is empty";
        return QJsonObject();
    }
    
    // Пытаемся исправить проблемы с кодировкой
    QString jsonString = QString::fromUtf8(data);
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "BoardMessagesJsonWriter: JSON parse error:" << error.errorString();
        qWarning() << "BoardMessagesJsonWriter: JSON content:" << jsonString.left(200);
        return QJsonObject();
    }
    
    if (doc.isObject()) {
        return doc.object();
    }
    
    return QJsonObject();
}

bool BoardMessagesJsonWriter::writeToFile(const QJsonObject &data)
{
    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "BoardMessagesJsonWriter: Cannot open file for writing:" << m_filename;
        return false;
    }
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    
    // Записываем данные с явным указанием кодировки UTF-8
    qint64 bytesWritten = file.write(jsonData);
    file.close();
    
    if (bytesWritten != jsonData.size()) {
        qWarning() << "BoardMessagesJsonWriter: Failed to write all data to file";
        return false;
    }
    
    return true;
}

QString BoardMessagesJsonWriter::generateBlockName(int blockNumber) const
{
    return QString("Message " + QString::number(blockNumber));
}
