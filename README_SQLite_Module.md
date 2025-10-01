# BoardMessagesSqliteWriter - Модуль для работы с SQLite базой данных

## Описание

`BoardMessagesSqliteWriter` - это новый модуль для эффективного хранения данных параметров платы в SQLite базе данных. Он заменяет `BoardMessagesJsonWriterNew` и предоставляет более производительное решение для работы с данными.

## Основные преимущества

1. **Производительность**: Данные записываются в базу данных напрямую, без постоянного чтения/записи файлов
2. **Множественные сессии**: Возможность сохранения и переключения между различными сессиями
3. **Компактное хранение**: Данные хранятся в нормализованном виде, что экономит место
4. **Быстрые запросы**: SQLite обеспечивает быстрый поиск и фильтрацию данных
5. **Транзакции**: Гарантированная целостность данных при записи

## Структура базы данных

### Таблица `sessions`
- `id` - Уникальный идентификатор сессии
- `name` - Название сессии
- `created_at` - Время создания сессии
- `description` - Описание сессии (опционально)

### Таблица `parameters`
- `id` - Уникальный идентификатор параметра
- `label` - Метка параметра (уникальная)
- `unit` - Единица измерения
- `created_at` - Время первого появления параметра

### Таблица `parameter_values`
- `id` - Уникальный идентификатор записи
- `session_id` - Ссылка на сессию
- `parameter_id` - Ссылка на параметр
- `value` - Значение параметра (в текстовом виде)
- `timestamp` - Временная метка значения

## Использование

### Базовое использование

```cpp
#include "BoardMessagesSqliteWriter.h"

// Создание объекта
BoardMessagesSqliteWriter *writer = new BoardMessagesSqliteWriter("data.db");

// Добавление сообщения
QList<BoardParameterSingle*> parameters;
// ... заполнение parameters
writer->addMessage(parameters);

// Принудительная запись
writer->flushQueue();
```

### Управление сессиями

```cpp
// Создание новой сессии
writer->createNewSession("Тестовая сессия");

// Переключение на сессию по ID
writer->switchToSession(1);

// Переключение на сессию по имени
writer->switchToSession("Тестовая сессия");

// Получение списка доступных сессий
QStringList sessions = writer->getAvailableSessions();

// Получение информации о текущей сессии
QString currentSession = writer->getCurrentSessionName();
int currentId = writer->getCurrentSessionId();
```

### Чтение данных

```cpp
#include "BoardMessagesSqliteReader.h"

BoardMessagesSqliteReader *reader = new BoardMessagesSqliteReader("data.db");

// Получение всех параметров сессии
QList<BoardParameterSingle*> params = reader->getSessionParameters(sessionId);

// Получение параметров конкретного типа
QList<BoardParameterSingle*> altitudeParams = reader->getSessionParameters(sessionId, "Altitude");

// Получение параметров за период времени
QDateTime start = QDateTime::currentDateTime().addDays(-1);
QDateTime end = QDateTime::currentDateTime();
QList<BoardParameterSingle*> recentParams = reader->getSessionParametersByTimeRange(sessionId, start, end);

// Экспорт в JSON
QString jsonData = reader->exportSessionToJson(sessionId);
```

## Миграция с BoardMessagesJsonWriterNew

### Шаг 1: Замена объекта
```cpp
// Старый код
BoardMessagesJsonWriterNew *writer = new BoardMessagesJsonWriterNew("data.json");

// Новый код
BoardMessagesSqliteWriter *writer = new BoardMessagesSqliteWriter("data.db");
```

### Шаг 2: Использование тех же методов
```cpp
// Эти методы работают аналогично
writer->addMessage(parameters);
writer->flushQueue();
writer->setWriteInterval(1000);
writer->clearFile(); // теперь clearCurrentSession()
```

### Шаг 3: Добавление новых возможностей
```cpp
// Создание именованных сессий
writer->createNewSession("Сессия от " + QDateTime::currentDateTime().toString());

// Переключение между сессиями
writer->switchToSession("Предыдущая сессия");

// Получение статистики
int messageCount = writer->getMessageCount();
int parameterCount = writer->getParameterCount();
```

## Производительность

### Сравнение с JSON подходом

| Операция | JSON Writer | SQLite Writer |
|----------|-------------|---------------|
| Запись сообщения | O(n) - чтение всего файла | O(1) - вставка в БД |
| Поиск по времени | O(n) - линейный поиск | O(log n) - индексный поиск |
| Фильтрация параметров | O(n) - полное сканирование | O(log n) - SQL запрос |
| Размер данных | Большой (дублирование) | Компактный (нормализация) |

### Рекомендации по настройке

1. **Интервал записи**: Рекомендуется 1000-5000 мс для баланса производительности и актуальности данных
2. **Размер очереди**: SQLite эффективно обрабатывает большие объемы данных
3. **Индексы**: Автоматически создаются для оптимизации запросов

## Обработка ошибок

Модуль предоставляет сигналы для обработки ошибок:

```cpp
connect(writer, &BoardMessagesSqliteWriter::writeError, 
        [](const QString &error) {
            qWarning() << "Ошибка записи:" << error;
        });

connect(reader, &BoardMessagesSqliteReader::readError, 
        [](const QString &error) {
            qWarning() << "Ошибка чтения:" << error;
        });
```

## Требования

- Qt 5.15+
- SQLite (входит в Qt)
- Модуль `sql` в .pro файле:
```pro
QT += sql
```

## Файлы модуля

- `BoardMessagesSqliteWriter.h` - Заголовочный файл писателя
- `BoardMessagesSqliteWriter.cpp` - Реализация писателя
- `BoardMessagesSqliteReader.h` - Заголовочный файл читателя
- `BoardMessagesSqliteReader.cpp` - Реализация читателя
- `BoardStationDataManager.cpp` - Пример интеграции
