#include "BoardMessagesSqliteWriter.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QApplication>
#include <QDir>

BoardMessagesSqliteWriter::BoardMessagesSqliteWriter(const QString &databasePath, QObject *parent)
    : QObject(parent)
    , m_databasePath(QApplication::applicationDirPath() + "/" + databasePath)
    , m_writeInterval(1000) // 1 секунда по умолчанию
    , m_currentSessionId(-1)
{
    // Выводим полный путь к базе данных для отладки
    qDebug() << "BoardMessagesSqliteWriter: Путь к базе данных:" << m_databasePath;
    qDebug() << "BoardMessagesSqliteWriter: Папка приложения:" << QApplication::applicationDirPath();
    
    m_writeTimer = new QTimer(this);
    connect(m_writeTimer, &QTimer::timeout, this, &BoardMessagesSqliteWriter::writeMessagesToDatabase);
    
    // Инициализируем базу данных
    if (initializeDatabase()) 
    {
        m_writeTimer->start(m_writeInterval);
        // Создаём сессию по умолчанию
        //createNewSession("Default Session");
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось инициализировать базу данных";
    }
}

BoardMessagesSqliteWriter::~BoardMessagesSqliteWriter()
{
    qDebug() << "BoardMessagesSqliteWriter: Деструктор - сохранение данных...";
    
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
    
    // Принудительно сохраняем все изменения в базу данных
    if (m_database.isOpen()) 
    {
        QSqlQuery query(m_database);
        query.exec("PRAGMA synchronous = FULL"); // Принудительная синхронизация
        query.exec("PRAGMA journal_mode = WAL");  // Режим журнала
        
        // Проверяем размер файла перед закрытием
        QFileInfo dbFile(m_databasePath);
        qDebug() << "BoardMessagesSqliteWriter: Размер файла перед закрытием:" << dbFile.size() << "байт";
        
        m_database.close();
        
        // Проверяем размер файла после закрытия
        qDebug() << "BoardMessagesSqliteWriter: Размер файла после закрытия:" << dbFile.size() << "байт";
        qDebug() << "BoardMessagesSqliteWriter: Файл существует:" << dbFile.exists();
    }
}

void BoardMessagesSqliteWriter::addMessage(const QList<BoardParameterSingle*> &parameters, 
                                         const QDateTime &timestamp)
{
    if (parameters.isEmpty()) 
    {
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
}

void BoardMessagesSqliteWriter::createNewSession(const QString &sessionName)
{
    QMutexLocker locker(&m_databaseMutex);
    
    QString name = sessionName;
    if (name.isEmpty())
    {
        QSqlQuery countQuery(m_database);
        countQuery.exec("SELECT COUNT(*) FROM sessions");
        int sessionCount = 0;
        if (countQuery.next())
        {
            sessionCount = countQuery.value(0).toInt();
        }
        name = QString("Session %1").arg(sessionCount + 1);
    }
    
    int sessionId = createSessionRecord(name);
    if (sessionId > 0) 
    {
        m_currentSessionId = sessionId;
        emit sessionCreated(sessionId, name);
        qDebug() << "BoardMessagesSqliteWriter: Создана новая сессия" << sessionId << "с именем" << name;
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось создать сессию";
        emit writeError("Не удалось создать новую сессию");
    }
}

void BoardMessagesSqliteWriter::switchToSession(int sessionId)
{
    QMutexLocker locker(&m_databaseMutex);
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name FROM sessions WHERE id = ?");
    query.addBindValue(sessionId);
    
    if (query.exec() && query.next()) 
    {
        m_currentSessionId = sessionId;
        QString sessionName = query.value("name").toString();
        emit sessionSwitched(sessionId, sessionName);
        qDebug() << "BoardMessagesSqliteWriter: Переключились на сессию" << sessionId << sessionName;
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Сессия с ID" << sessionId << "не найдена";
        emit writeError(QString("Сессия с ID %1 не найдена").arg(sessionId));
    }
}

void BoardMessagesSqliteWriter::switchToSession(const QString &sessionName)
{
    QMutexLocker locker(&m_databaseMutex);
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM sessions WHERE name = ?");
    query.addBindValue(sessionName);
    
    if (query.exec() && query.next()) 
    {
        int sessionId = query.value("id").toInt();
        m_currentSessionId = sessionId;
        emit sessionSwitched(sessionId, sessionName);
        qDebug() << "BoardMessagesSqliteWriter: Переключились на сессию" << sessionId << sessionName;
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Сессия с именем" << sessionName << "не найдена";
        emit writeError(QString("Сессия с именем '%1' не найдена").arg(sessionName));
    }
}

QStringList BoardMessagesSqliteWriter::getAvailableSessions() const
{
    QMutexLocker locker(&m_databaseMutex);
    QStringList sessions;
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, created_at FROM sessions ORDER BY created_at DESC");
    
    if (query.exec()) 
    {
        while (query.next()) 
        {
            int id = query.value("id").toInt();
            QString name = query.value("name").toString();
            QDateTime createdAt = query.value("created_at").toDateTime();
            sessions.append(QString("%1: %2 (%3)")
                          .arg(id)
                          .arg(name)
                          .arg(createdAt.toString("yyyy-MM-dd hh:mm:ss")));
        }
    }
    
    return sessions;
}

QString BoardMessagesSqliteWriter::getCurrentSessionName() const
{
    QMutexLocker locker(&m_databaseMutex);
    
    if (m_currentSessionId <= 0) 
    {
        return QString();
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT name FROM sessions WHERE id = ?");
    query.addBindValue(m_currentSessionId);
    
    if (query.exec() && query.next()) 
    {
        return query.value("name").toString();
    }
    
    return QString();
}

void BoardMessagesSqliteWriter::flushQueue()
{
    writeMessagesToDatabase();
}

void BoardMessagesSqliteWriter::forceSave()
{
    qDebug() << "BoardMessagesSqliteWriter: Принудительное сохранение...";
    
    // Записываем все сообщения из очереди
    flushQueue();
    
    // Принудительно синхронизируем с диском
    if (m_database.isOpen()) 
    {
        QSqlQuery query(m_database);
        query.exec("PRAGMA synchronous = FULL");
        query.exec("PRAGMA journal_mode = WAL");
        
        // Проверяем размер файла
        QFileInfo dbFile(m_databasePath);
        qDebug() << "BoardMessagesSqliteWriter: После принудительного сохранения - размер файла:" << dbFile.size() << "байт";
        qDebug() << "BoardMessagesSqliteWriter: Файл существует:" << dbFile.exists();
    }
}

void BoardMessagesSqliteWriter::setWriteInterval(int intervalMs)
{
    m_writeInterval = intervalMs;
    if (m_writeTimer)
    {
        m_writeTimer->setInterval(intervalMs);
    }
}

void BoardMessagesSqliteWriter::clearCurrentSession()
{
    if (m_currentSessionId <= 0) 
    {
        emit writeError("Нет активной сессии для очистки");
        return;
    }
    
    QMutexLocker locker(&m_databaseMutex);
    
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM parameter_values WHERE session_id = ?");
    query.addBindValue(m_currentSessionId);
    
    if (query.exec()) 
    {
        emit sessionCleared(m_currentSessionId);
        qDebug() << "BoardMessagesSqliteWriter: Очищены данные сессии" << m_currentSessionId;
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка очистки сессии:" << query.lastError().text();
        emit writeError("Ошибка очистки данных сессии");
    }
}

int BoardMessagesSqliteWriter::getMessageCount() const
{
    QMutexLocker locker(&m_databaseMutex);
    
    if (m_currentSessionId <= 0) 
    {
        return 0;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(DISTINCT timestamp) FROM parameter_values WHERE session_id = ?");
    query.addBindValue(m_currentSessionId);
    
    if (query.exec() && query.next()) 
    {
        return query.value(0).toInt();
    }
    
    return 0;
}

int BoardMessagesSqliteWriter::getParameterCount() const
{
    QMutexLocker locker(&m_databaseMutex);
    
    if (m_currentSessionId <= 0) 
    {
        return 0;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM parameter_values WHERE session_id = ?");
    query.addBindValue(m_currentSessionId);
    
    if (query.exec() && query.next()) 
    {
        return query.value(0).toInt();
    }
    
    return 0;
}

void BoardMessagesSqliteWriter::writeMessagesToDatabase()
{
    QMutexLocker locker(&m_queueMutex);
    
    if (m_messageQueue.isEmpty() || m_currentSessionId <= 0)
    {
        return;
    }
    
    int messageCount = m_messageQueue.size();
    
    // Записываем все сообщения из очереди
    while (!m_messageQueue.isEmpty())
    {
        ParameterMessage *message = m_messageQueue.dequeue();
        
        if (!writeMessageToDatabase(message))
        {
            qWarning() << "BoardMessagesSqliteWriter: Ошибка записи сообщения в базу данных";
            emit writeError("Ошибка записи сообщения в базу данных");
        }
        
        // Удаляем сообщение после использования
        qDeleteAll(message->parameters);
        delete message;
    }
    
    emit writeSuccess(QString("Записано %1 сообщений в базу данных").arg(messageCount));
    
    // Проверяем размер файла после записи
    QFileInfo dbFile(m_databasePath);
    qDebug() << "BoardMessagesSqliteWriter: После записи - размер файла:" << dbFile.size() << "байт";
}

bool BoardMessagesSqliteWriter::initializeDatabase()
{
    // Проверяем, существует ли файл базы данных
    QFileInfo dbFile(m_databasePath);
    bool fileExisted = dbFile.exists();
    
    // Устанавливаем соединение с базой данных
    m_database = QSqlDatabase::addDatabase("QSQLITE", "BoardStationConnection");
    m_database.setDatabaseName(m_databasePath);
    
    if (!m_database.open()) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось открыть базу данных:" << m_database.lastError().text();
        qWarning() << "BoardMessagesSqliteWriter: Путь к файлу:" << m_databasePath;
        return false;
    }
    
    // Создаём таблицы
    if (!createTables()) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось создать таблицы";
        return false;
    }
    
    // Проверяем, что файл действительно создался
    if (!fileExisted && !dbFile.exists()) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Файл базы данных не был создан!";
        return false;
    }
    
    qDebug() << "BoardMessagesSqliteWriter: База данных инициализирована:" << m_databasePath;
    qDebug() << "BoardMessagesSqliteWriter: Файл существует:" << dbFile.exists();
    qDebug() << "BoardMessagesSqliteWriter: Размер файла:" << dbFile.size() << "байт";
    return true;
}

bool BoardMessagesSqliteWriter::createTables()
{
    QSqlQuery query(m_database);
    
    // Таблица сессий
    QString createSessionsTable = R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            description TEXT
        )
    )";
    
    if (!query.exec(createSessionsTable)) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка создания таблицы sessions:" << query.lastError().text();
        return false;
    }
    
    // Таблица параметров (справочник)
    QString createParametersTable = R"(
        CREATE TABLE IF NOT EXISTS parameters (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            label TEXT UNIQUE NOT NULL,
            unit TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createParametersTable)) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка создания таблицы parameters:" << query.lastError().text();
        return false;
    }
    
    // Таблица значений параметров
    QString createParameterValuesTable = R"(
        CREATE TABLE IF NOT EXISTS parameter_values (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id INTEGER NOT NULL,
            parameter_id INTEGER NOT NULL,
            value TEXT NOT NULL,
            timestamp DATETIME NOT NULL,
            FOREIGN KEY (session_id) REFERENCES sessions (id),
            FOREIGN KEY (parameter_id) REFERENCES parameters (id)
        )
    )";
    
    if (!query.exec(createParameterValuesTable)) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка создания таблицы parameter_values:" << query.lastError().text();
        return false;
    }
    
    // Создаём индексы для оптимизации запросов
    query.exec("CREATE INDEX IF NOT EXISTS idx_parameter_values_session_timestamp ON parameter_values (session_id, timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_parameter_values_parameter ON parameter_values (parameter_id)");
    
    return true;
}

int BoardMessagesSqliteWriter::createSessionRecord(const QString &sessionName)
{
    QSqlQuery query(m_database);
    // Явно указываем локальное время создания вместо DEFAULT CURRENT_TIMESTAMP
    query.prepare("INSERT INTO sessions (name, created_at) VALUES (?, ?)");
    query.addBindValue(sessionName);
    query.addBindValue(QDateTime::currentDateTime().toLocalTime());
    
    if (query.exec()) 
    {
        return query.lastInsertId().toInt();
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка создания записи сессии:" << query.lastError().text();
        return -1;
    }
}

bool BoardMessagesSqliteWriter::writeMessageToDatabase(const ParameterMessage *message)
{
    QMutexLocker locker(&m_databaseMutex);
    
    // Начинаем транзакцию для атомарности записи
    if (!m_database.transaction()) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось начать транзакцию";
        return false;
    }
    
    QSqlQuery query(m_database);
    
    // Записываем каждый параметр из сообщения
    for (const BoardParameterSingle *param : message->parameters) 
    {
        if (!param) continue;
        
        // Получаем ID параметра (создаём при необходимости)
        int parameterId = getParameterId(param->label(), param->unit());
        if (parameterId <= 0) 
        {
            qWarning() << "BoardMessagesSqliteWriter: Не удалось получить ID параметра:" << param->label();
            m_database.rollback();
            return false;
        }
        
        // Записываем значение параметра
        query.prepare("INSERT INTO parameter_values (session_id, parameter_id, value, timestamp) VALUES (?, ?, ?, ?)");
        query.addBindValue(m_currentSessionId);
        query.addBindValue(parameterId);
        query.addBindValue(param->value().toString());
        // Сохраняем временную метку в локальном времени
        query.addBindValue(message->timestamp.toLocalTime());
        
        if (!query.exec()) 
        {
            qWarning() << "BoardMessagesSqliteWriter: Ошибка записи значения параметра:" << query.lastError().text();
            m_database.rollback();
            return false;
        }
    }
    
    // Подтверждаем транзакцию
    if (!m_database.commit()) 
    {
        qWarning() << "BoardMessagesSqliteWriter: Не удалось подтвердить транзакцию";
        return false;
    }
    
    return true;
}

int BoardMessagesSqliteWriter::getParameterId(const QString &label, const QString &unit)
{
    QSqlQuery query(m_database);
    
    // Сначала пытаемся найти существующий параметр
    query.prepare("SELECT id FROM parameters WHERE label = ?");
    query.addBindValue(label);
    
    if (query.exec() && query.next()) 
    {
        return query.value("id").toInt();
    }
    
    // Параметр не найден, создаём новый
    query.prepare("INSERT INTO parameters (label, unit) VALUES (?, ?)");
    query.addBindValue(label);
    query.addBindValue(unit);
    
    if (query.exec()) 
    {
        return query.lastInsertId().toInt();
    }
    else
    {
        qWarning() << "BoardMessagesSqliteWriter: Ошибка создания параметра:" << query.lastError().text();
        return -1;
    }
}
