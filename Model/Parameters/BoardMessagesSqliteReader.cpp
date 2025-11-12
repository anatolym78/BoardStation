#include "BoardMessagesSqliteReader.h"
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QApplication>
#include "Model/Parameters/Tree/ParameterTreeGroupItem.h"
#include "Model/Parameters/Tree/ParameterTreeArrayItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

BoardMessagesSqliteReader::BoardMessagesSqliteReader(const QString &databasePath, QObject *parent)
    : QObject(parent)
    , m_databasePath(QApplication::applicationDirPath() + "/" + databasePath)
{
    initializeDatabase();
}

BoardMessagesSqliteReader::~BoardMessagesSqliteReader()
{
    if (m_database.isOpen()) 
    {
        m_database.close();
    }
}

QList<BoardMessagesSqliteReader::SessionInfo> BoardMessagesSqliteReader::getAvailableSessions()
{
    QList<SessionInfo> sessions;
    
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT s.id, s.name, s.created_at, s.description,
               COUNT(DISTINCT pv.timestamp) as message_count,
               COUNT(pv.id) as parameter_count
        FROM sessions s
        LEFT JOIN parameter_values pv ON s.id = pv.session_id
        GROUP BY s.id, s.name, s.created_at, s.description
        ORDER BY s.created_at DESC
    )");
    
    if (query.exec()) 
    {
        while (query.next()) 
        {
            SessionInfo info;
            info.id = query.value("id").toInt();
            info.name = query.value("name").toString();
            // Время теперь сохраняется в локальном времени, поэтому просто читаем его
            info.createdAt = query.value("created_at").toDateTime();
            info.description = query.value("description").toString();
            info.messageCount = query.value("message_count").toInt();
            info.parameterCount = query.value("parameter_count").toInt();
            sessions.append(info);
        }
    }
    else
    {
        qWarning() << "BoardMessagesSqliteReader: Ошибка получения списка сессий:" << query.lastError().text();
        emit readError("Ошибка получения списка сессий");
    }
    
    return sessions;
}

QList<BoardParameterSingle*> BoardMessagesSqliteReader::getSessionParameters(int sessionId, 
                                                                            const QString &parameterLabel)
{
    QList<BoardParameterSingle*> parameters;
    
    QSqlQuery query(m_database);
    
    QString sql = R"(
        SELECT p.label, p.unit, pv.value, pv.timestamp
        FROM parameter_values pv
        JOIN parameters p ON pv.parameter_id = p.id
        WHERE pv.session_id = ?
    )";
    
    if (!parameterLabel.isEmpty()) 
    {
        sql += " AND p.label = ?";
    }
    
    sql += " ORDER BY pv.timestamp ASC, p.label ASC";
    
    query.prepare(sql);
    query.addBindValue(sessionId);
    
    if (!parameterLabel.isEmpty()) 
    {
        query.addBindValue(parameterLabel);
    }
    
    if (query.exec()) 
    {
        while (query.next()) 
        {
            BoardParameterSingle *param = createParameterFromQuery(query);
            if (param) 
            {
                parameters.append(param);
            }
        }
    }
    else
    {
        qWarning() << "BoardMessagesSqliteReader: Ошибка получения параметров сессии:" << query.lastError().text();
        emit readError("Ошибка получения параметров сессии");
    }
    
    return parameters;
}

QList<BoardParameterSingle*> BoardMessagesSqliteReader::getSessionParametersByTimeRange(int sessionId,
                                                                                        const QDateTime &startTime,
                                                                                        const QDateTime &endTime,
                                                                                        const QString &parameterLabel)
{
    QList<BoardParameterSingle*> parameters;
    
    QSqlQuery query(m_database);
    
    QString sql = R"(
        SELECT p.label, p.unit, pv.value, pv.timestamp
        FROM parameter_values pv
        JOIN parameters p ON pv.parameter_id = p.id
        WHERE pv.session_id = ? AND pv.timestamp >= ? AND pv.timestamp <= ?
    )";
    
    if (!parameterLabel.isEmpty()) 
    {
        sql += " AND p.label = ?";
    }
    
    sql += " ORDER BY pv.timestamp ASC, p.label ASC";
    
    query.prepare(sql);
    query.addBindValue(sessionId);
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    
    if (!parameterLabel.isEmpty()) 
    {
        query.addBindValue(parameterLabel);
    }
    
    if (query.exec()) 
    {
        while (query.next()) 
        {
            BoardParameterSingle *param = createParameterFromQuery(query);
            if (param) 
            {
                parameters.append(param);
            }
        }
    }
    else
    {
        qWarning() << "BoardMessagesSqliteReader: Ошибка получения параметров по времени:" << query.lastError().text();
        emit readError("Ошибка получения параметров по времени");
    }
    
    return parameters;
}

QStringList BoardMessagesSqliteReader::getSessionParameterLabels(int sessionId)
{
    QStringList labels;
    
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT DISTINCT p.label
        FROM parameter_values pv
        JOIN parameters p ON pv.parameter_id = p.id
        WHERE pv.session_id = ?
        ORDER BY p.label
    )");
    query.addBindValue(sessionId);
    
    if (query.exec()) 
    {
        while (query.next()) 
        {
            labels.append(query.value("label").toString());
        }
    }
    else
    {
        qWarning() << "BoardMessagesSqliteReader: Ошибка получения меток параметров:" << query.lastError().text();
        emit readError("Ошибка получения меток параметров");
    }
    
    return labels;
}

BoardMessagesSqliteReader::SessionInfo BoardMessagesSqliteReader::getSessionInfo(int sessionId)
{
    SessionInfo info;
    info.id = -1;
    
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT s.id, s.name, s.created_at, s.description,
               COUNT(DISTINCT pv.timestamp) as message_count,
               COUNT(pv.id) as parameter_count
        FROM sessions s
        LEFT JOIN parameter_values pv ON s.id = pv.session_id
        WHERE s.id = ?
        GROUP BY s.id, s.name, s.created_at, s.description
    )");
    query.addBindValue(sessionId);
    
    if (query.exec() && query.next()) 
    {
        info.id = query.value("id").toInt();
        info.name = query.value("name").toString();
        // Время теперь сохраняется в локальном времени, поэтому просто читаем его
        info.createdAt = query.value("created_at").toDateTime();
        info.description = query.value("description").toString();
        info.messageCount = query.value("message_count").toInt();
        info.parameterCount = query.value("parameter_count").toInt();
    }
    else
    {
        qWarning() << "BoardMessagesSqliteReader: Ошибка получения информации о сессии:" << query.lastError().text();
        emit readError("Ошибка получения информации о сессии");
    }
    
    return info;
}

QString BoardMessagesSqliteReader::exportSessionToJson(int sessionId)
{
    QJsonObject rootObject;
    
    // Получаем информацию о сессии
    SessionInfo sessionInfo = getSessionInfo(sessionId);
    if (sessionInfo.id == -1) 
    {
        emit readError("Сессия не найдена");
        return QString();
    }
    
    rootObject["session_id"] = sessionInfo.id;
    rootObject["session_name"] = sessionInfo.name;
    rootObject["created_at"] = sessionInfo.createdAt.toString(Qt::ISODate);
    rootObject["message_count"] = sessionInfo.messageCount;
    rootObject["parameter_count"] = sessionInfo.parameterCount;
    
    // Получаем все параметры сессии
    QList<BoardParameterSingle*> parameters = getSessionParameters(sessionId);
    
    // Группируем параметры по временным меткам
    QMap<QDateTime, QJsonArray> messagesByTimestamp;
    
    for (BoardParameterSingle *param : parameters) 
    {
        if (!param) continue;
        
        QJsonObject paramObj;
        paramObj["label"] = param->label();
        paramObj["value"] = QJsonValue::fromVariant(param->value());
        paramObj["unit"] = param->unit();
        paramObj["timestamp"] = param->timestamp().toString(Qt::ISODate);
        
        messagesByTimestamp[param->timestamp()].append(paramObj);
    }
    
    // Создаём массив сообщений
    QJsonArray messagesArray;
    for (auto it = messagesByTimestamp.begin(); it != messagesByTimestamp.end(); ++it) 
    {
        QJsonObject messageObj;
        messageObj["timestamp"] = it.key().toString(Qt::ISODate);
        messageObj["parameters"] = it.value();
        messagesArray.append(messageObj);
    }
    
    rootObject["messages"] = messagesArray;
    
    // Очищаем память
    qDeleteAll(parameters);
    
    QJsonDocument doc(rootObject);
    return doc.toJson(QJsonDocument::Indented);
}

bool BoardMessagesSqliteReader::initializeDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", "BoardStationReaderConnection");
    m_database.setDatabaseName(m_databasePath);
    
    if (!m_database.open()) 
    {
        qWarning() << "BoardMessagesSqliteReader: Не удалось открыть базу данных:" << m_database.lastError().text();
        emit readError("Не удалось открыть базу данных");
        return false;
    }
    
    return true;
}

BoardParameterSingle* BoardMessagesSqliteReader::createParameterFromQuery(const QSqlQuery &query) const
{
    QString label = query.value("label").toString();
    QString unit = query.value("unit").toString();
    QString valueStr = query.value("value").toString();
    // Время теперь сохраняется в локальном времени, поэтому просто читаем его
    QDateTime timestamp = query.value("timestamp").toDateTime();
    
    // Пытаемся определить тип значения
    QVariant value;
    
    // Проверяем, является ли значение числом
    bool ok;
    double doubleValue = valueStr.toDouble(&ok);
    if (ok) 
    {
        value = doubleValue;
    }
    else
    {
        // Проверяем, является ли значение булевым
        if (valueStr.toLower() == "true" || valueStr.toLower() == "false") 
        {
            value = valueStr.toLower() == "true";
        }
        else
        {
            // Оставляем как строку
            value = valueStr;
        }
    }
    
    return new BoardParameterSingle(label, value, timestamp, unit, const_cast<BoardMessagesSqliteReader*>(this));
}

bool BoardMessagesSqliteReader::removeSession(int sessionId)
{
    if (sessionId <= 0)
    {
        qWarning() << "BoardMessagesSqliteReader: Invalid session ID for removal:" << sessionId;
        return false;
    }
    
    QSqlQuery query(m_database);
    
    // Начинаем транзакцию
    m_database.transaction();
    
    try 
    {
        // Удаляем все значения параметров для этой сессии
        query.prepare("DELETE FROM parameter_values WHERE session_id = ?");
        query.addBindValue(sessionId);
        
        if (!query.exec()) 
        {
            qWarning() << "BoardMessagesSqliteReader: Failed to delete parameter values for session" << sessionId << ":" << query.lastError().text();
            m_database.rollback();
            return false;
        }
        
        // Удаляем саму сессию
        query.prepare("DELETE FROM sessions WHERE id = ?");
        query.addBindValue(sessionId);
        
        if (!query.exec()) 
        {
            qWarning() << "BoardMessagesSqliteReader: Failed to delete session" << sessionId << ":" << query.lastError().text();
            m_database.rollback();
            return false;
        }
        
        // Подтверждаем транзакцию
        m_database.commit();
        
        qDebug() << "BoardMessagesSqliteReader: Successfully removed session" << sessionId;
        return true;
    }
    catch (const std::exception &e) 
    {
        qWarning() << "BoardMessagesSqliteReader: Exception during session removal:" << e.what();
        m_database.rollback();
        return false;
    }
}

bool BoardMessagesSqliteReader::loadSessionToTree(int sessionId, ParameterTreeStorage* storage)
{
	if (!storage)
	{
		qWarning() << "BoardMessagesSqliteReader: loadSessionToTree - storage is null";
		return false;
	}

	// Очищаем текущее дерево
	storage->clear();

	QSqlQuery query(m_database);
	query.prepare(R"(
		SELECT p.label, p.unit, pv.value, pv.timestamp
		FROM parameter_values pv
		JOIN parameters p ON pv.parameter_id = p.id
		WHERE pv.session_id = ?
		ORDER BY pv.timestamp ASC, p.label ASC
	)");
	query.addBindValue(sessionId);

	if (!query.exec())
	{
		qWarning() << "BoardMessagesSqliteReader: Ошибка загрузки сессии в дерево:" << query.lastError().text();
		emit readError("Ошибка загрузки сессии в дерево");
		return false;
	}

	// Восстанавливаем узлы-истории и добавляем значения
	while (query.next())
	{
		const QString label = query.value("label").toString();
		const QString valueStr = query.value("value").toString();
		const QDateTime ts = query.value("timestamp").toDateTime();

		// Парсим значение как число/булево/строку (как при createParameterFromQuery)
		QVariant value;
		bool ok = false;
		const double dbl = valueStr.toDouble(&ok);
		if (ok)
		{
			value = dbl;
		}
		else
		{
			const QString lower = valueStr.toLower();
			if (lower == "true" || lower == "false")
			{
				value = (lower == "true");
			}
			else
			{
				value = valueStr;
			}
		}

		// Получаем части составной метки
		const QStringList parts = BoardParameterSingle(label).labelParts();
		if (parts.isEmpty()) continue;

		auto historyItem = ensureHistoryPath(storage, parts);
		if (!historyItem) continue;
		historyItem->addValue(value, ts);
	}

	return true;
}

ParameterTreeHistoryItem* BoardMessagesSqliteReader::ensureHistoryPath(ParameterTreeStorage* storage, const QStringList& labelParts) const
{
	if (!storage || labelParts.isEmpty()) return nullptr;

	ParameterTreeItem* current = storage;
	// Все части пути, кроме последней, считаем группами
	for (int i = 0; i < labelParts.size() - 1; ++i)
	{
		const QString& part = labelParts.at(i);
		auto existing = current->findChildByLabel(part, false);
		if (!existing)
		{
			// По умолчанию создаем как Group, но если узел уже существует как Array, используем его
			// При установке снимка через setSnapshot типы будут исправлены при несовпадении
			auto group = new ParameterTreeGroupItem(part, current);
			current->appendChild(group);
			current = group;
		}
		else
		{
			current = existing;
		}
	}

	// Последняя часть — узел-история
	const QString& leaf = labelParts.last();
	auto existingLeaf = current->findChildByLabel(leaf, false);
	if (existingLeaf && existingLeaf->type() == ParameterTreeItem::ItemType::History)
	{
		return static_cast<ParameterTreeHistoryItem*>(existingLeaf);
	}

	if (!existingLeaf)
	{
		auto history = new ParameterTreeHistoryItem(leaf, current);
		current->appendChild(history);
		return history;
	}

	// Если по какой-то причине лист существует, но не History — создадим новый History с уникальным именем
	auto history = new ParameterTreeHistoryItem(leaf + "_hist", current);
	current->appendChild(history);
	return history;
}