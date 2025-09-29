#ifndef DEBUGVIEWMODEL_H
#define DEBUGVIEWMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QString>

class DebugViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum class DebugRole : int
    {
        TimestampRole = Qt::UserRole + 1,
        MessageRole,
        TypeRole,
        FormattedMessageRole
    };

    enum class MessageType
    {
        UplinkParameters,
        DataReceived,
        Error,
        Info
    };

public:
    explicit DebugViewModel(QObject *parent = nullptr);
    ~DebugViewModel() override = default;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    // Методы для добавления сообщений
    void addUplinkParametersMessage(const QString &data);
    void addDataReceivedMessage(const QString &data);
    void addErrorMessage(const QString &message);
    void addInfoMessage(const QString &message);
    
    // Методы управления
    void clearMessages();
    int getMessageCount() const { return m_messages.size(); }

private:
    struct DebugMessage
    {
        QDateTime timestamp;
        QString message;
        MessageType type;
        
        DebugMessage(const QDateTime &ts, const QString &msg, MessageType t)
            : timestamp(ts), message(msg), type(t) {}
    };

    void addMessage(const QString &message, MessageType type);
    QString formatMessage(const DebugMessage &msg) const;

    QList<DebugMessage> m_messages;
    static const int MAX_MESSAGES = 1000; // Максимальное количество сообщений
};

#endif // DEBUGVIEWMODEL_H
