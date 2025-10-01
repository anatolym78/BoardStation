/*
 * Пример использования BoardMessagesSqliteWriter
 * 
 * Этот файл демонстрирует, как заменить BoardMessagesJsonWriterNew
 * на новый SQLite-based модуль для более эффективного хранения данных.
 */

#include "BoardMessagesSqliteWriter.h"
#include "BoardMessagesSqliteReader.h"
#include <QDebug>

class BoardStationDataManager : public QObject
{
    Q_OBJECT

public:
    explicit BoardStationDataManager(QObject *parent = nullptr)
        : QObject(parent)
        , m_writer(new BoardMessagesSqliteWriter("BoardStationData.db", this))
        , m_reader(new BoardMessagesSqliteReader("BoardStationData.db", this))
    {
        // Подключаем сигналы для мониторинга состояния
        connect(m_writer, &BoardMessagesSqliteWriter::writeError, 
                this, &BoardStationDataManager::onWriteError);
        connect(m_writer, &BoardMessagesSqliteWriter::writeSuccess, 
                this, &BoardStationDataManager::onWriteSuccess);
        connect(m_writer, &BoardMessagesSqliteWriter::sessionCreated, 
                this, &BoardStationDataManager::onSessionCreated);
        connect(m_writer, &BoardMessagesSqliteWriter::sessionSwitched, 
                this, &BoardStationDataManager::onSessionSwitched);
        
        connect(m_reader, &BoardMessagesSqliteReader::readError, 
                this, &BoardStationDataManager::onReadError);
    }
    
    // Методы для работы с данными (аналогично BoardMessagesJsonWriterNew)
    void addMessage(const QList<BoardParameterSingle*> &parameters, 
                   const QDateTime &timestamp = QDateTime::currentDateTime())
    {
        m_writer->addMessage(parameters, timestamp);
    }
    
    void clearCurrentSession()
    {
        m_writer->clearCurrentSession();
    }
    
    void flushQueue()
    {
        m_writer->flushQueue();
    }
    
    void setWriteInterval(int intervalMs)
    {
        m_writer->setWriteInterval(intervalMs);
    }
    
    // Новые методы для работы с сессиями
    void createNewSession(const QString &sessionName = QString())
    {
        m_writer->createNewSession(sessionName);
    }
    
    void switchToSession(int sessionId)
    {
        m_writer->switchToSession(sessionId);
    }
    
    void switchToSession(const QString &sessionName)
    {
        m_writer->switchToSession(sessionName);
    }
    
    QStringList getAvailableSessions()
    {
        return m_writer->getAvailableSessions();
    }
    
    QString getCurrentSessionName()
    {
        return m_writer->getCurrentSessionName();
    }
    
    // Методы для чтения данных
    QList<BoardParameterSingle*> getCurrentSessionParameters(const QString &parameterLabel = QString())
    {
        return m_reader->getSessionParameters(m_writer->getCurrentSessionId(), parameterLabel);
    }
    
    QStringList getCurrentSessionParameterLabels()
    {
        return m_reader->getSessionParameterLabels(m_writer->getCurrentSessionId());
    }
    
    QString exportCurrentSessionToJson()
    {
        return m_reader->exportSessionToJson(m_writer->getCurrentSessionId());
    }
    
    // Статистика
    int getMessageCount()
    {
        return m_writer->getMessageCount();
    }
    
    int getParameterCount()
    {
        return m_writer->getParameterCount();
    }

private slots:
    void onWriteError(const QString &error)
    {
        qWarning() << "BoardStationDataManager: Ошибка записи:" << error;
    }
    
    void onWriteSuccess(const QString &message)
    {
        qDebug() << "BoardStationDataManager: Успешная запись:" << message;
    }
    
    void onSessionCreated(int sessionId, const QString &sessionName)
    {
        qDebug() << "BoardStationDataManager: Создана сессия" << sessionId << "с именем" << sessionName;
    }
    
    void onSessionSwitched(int sessionId, const QString &sessionName)
    {
        qDebug() << "BoardStationDataManager: Переключились на сессию" << sessionId << sessionName;
    }
    
    void onReadError(const QString &error)
    {
        qWarning() << "BoardStationDataManager: Ошибка чтения:" << error;
    }

private:
    BoardMessagesSqliteWriter *m_writer;
    BoardMessagesSqliteReader *m_reader;
};

/*
 * Пример миграции с BoardMessagesJsonWriterNew на BoardMessagesSqliteWriter:
 * 
 * 1. Замените в коде:
 *    BoardMessagesJsonWriterNew *writer = new BoardMessagesJsonWriterNew("data.json");
 *    на:
 *    BoardStationDataManager *dataManager = new BoardStationDataManager();
 * 
 * 2. Используйте те же методы:
 *    dataManager->addMessage(parameters);
 *    dataManager->flushQueue();
 *    dataManager->setWriteInterval(1000);
 * 
 * 3. Добавьте новые возможности:
 *    dataManager->createNewSession("Тестовая сессия");
 *    dataManager->switchToSession("Тестовая сессия");
 *    QStringList sessions = dataManager->getAvailableSessions();
 * 
 * 4. Для чтения данных:
 *    QList<BoardParameterSingle*> params = dataManager->getCurrentSessionParameters();
 *    QString jsonData = dataManager->exportCurrentSessionToJson();
 */

#include "BoardStationDataManager.moc"
