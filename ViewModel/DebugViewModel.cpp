#include "DebugViewModel.h"
#include <QDebug>
#include <QIcon>

DebugViewModel::DebugViewModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DebugViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_messages.size();
}


QHash<int, QByteArray> DebugViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[(int)DebugRole::TimestampRole] = "timestamp";
    roles[(int)DebugRole::MessageRole] = "message";
    roles[(int)DebugRole::TypeRole] = "messageType";
    roles[(int)DebugRole::FormattedMessageRole] = "formattedMessage";
    return roles;
}

QVariant DebugViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    const DebugMessage &message = m_messages[index.row()];

    // Обрабатываем стандартную роль DisplayRole для отображения в QListView
    if (role == Qt::DisplayRole)
    {
        return formatMessage(message);
    }

    // Обрабатываем роль DecorationRole для отображения иконок
    if (role == Qt::DecorationRole)
    {
        if (message.type == MessageType::Info)
        {
            return QIcon(":/Resources/info_16.png");
        }
        return QVariant();
    }

    // Обрабатываем кастомные роли только если role >= Qt::UserRole
    if (role < Qt::UserRole)
    {
        return QVariant();
    }

    auto debugRole = (DebugRole)role;

    switch (debugRole)
    {
    case DebugRole::TimestampRole:
        return message.timestamp.toString("hh:mm:ss.zzz");
    case DebugRole::MessageRole:
        return message.message;
    case DebugRole::TypeRole:
        switch (message.type)
        {
        case MessageType::UplinkParameters:
            return tr("Uplink");
        case MessageType::DataReceived:
            return tr("Received");
        case MessageType::Error:
            return tr("Error");
        case MessageType::Info:
            return tr("Info");
        }
        break;
    case DebugRole::FormattedMessageRole:
        return formatMessage(message);
    }

    return QVariant();
}


void DebugViewModel::addUplinkParametersMessage(const QString &data)
{
    addMessage(data, MessageType::UplinkParameters);
}

void DebugViewModel::addDataReceivedMessage(const QString &data)
{
    addMessage(data, MessageType::DataReceived);
}

void DebugViewModel::addErrorMessage(const QString &message)
{
    addMessage(message, MessageType::Error);
}

void DebugViewModel::addInfoMessage(const QString &message)
{
    addMessage(message, MessageType::Info);
}

void DebugViewModel::clearMessages()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();
}

QString DebugViewModel::formatMessage(const DebugMessage &msg) const
{
    QString typeStr;
    switch (msg.type)
    {
    case MessageType::UplinkParameters:
        typeStr = tr("[Uplink]");
        break;
    case MessageType::DataReceived:
        typeStr = tr("[Received]");
        break;
    case MessageType::Error:
        typeStr = tr("[Error]");
        break;
    case MessageType::Info:
        typeStr = tr("[Info]");
        break;
    }

    //return msg.message;
    
    return QString("%1 %2\n %3")
        .arg(msg.timestamp.toString("hh:mm:ss.zzz"))
        .arg(typeStr)
        .arg(msg.message);
}

void DebugViewModel::addMessage(const QString &message, MessageType type)
{
    // Ограничиваем количество сообщений
    if (m_messages.size() >= MAX_MESSAGES)
    {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_messages.removeFirst();
        endRemoveRows();
    }

    // Добавляем новое сообщение
    int row = m_messages.size();
    beginInsertRows(QModelIndex(), row, row);
    m_messages.append(DebugMessage(QDateTime::currentDateTime(), message, type));
    endInsertRows();
}
