#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QDateTime>
#include <QString>

/**
 * @brief Базовый класс для представления сессии
 */
class Session : public QObject
{
    Q_OBJECT

public:
    enum SessionType
    {
        RecordedSession,
        LiveSession
    };

    explicit Session(QObject *parent = nullptr);
    virtual ~Session() = default;

    // Виртуальные методы для получения информации о сессии
    virtual int getId() const = 0;
    virtual QString getName() const = 0;
    virtual QDateTime getCreatedAt() const = 0;
    virtual QString getDescription() const = 0;
    virtual int getMessageCount() const = 0;
    virtual int getParameterCount() const = 0;
    virtual SessionType getType() const = 0;
    virtual bool isRecording() const = 0;

    // Методы для сравнения сессий (для сортировки)
    virtual bool operator<(const Session& other) const;
    virtual bool operator==(const Session& other) const;

signals:
    void sessionChanged();
    void messageCountChanged(int count);
    void parameterCountChanged(int count);

protected:
    // Защищенные методы для обновления данных
    void updateMessageCount(int count);
    void updateParameterCount(int count);
};

#endif // SESSION_H
