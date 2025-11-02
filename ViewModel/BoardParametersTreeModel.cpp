#include "BoardParametersTreeModel.h"
#include "BoardParameterTreeItem.h"

#include <random>

BoardParametersTreeModel::BoardParametersTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    m_rootItem = new BoardParameterTreeItem("root");
    makeRandomColors();

    m_topLevelItems.insert("param", new BoardParameterTreeItem("label"));
}

void BoardParametersTreeModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    return;


    if (parameter == nullptr) return;

    QString label = parameter->label();
    QStringList parts = label.split('.');

    BoardParameterTreeItem* parentItem = m_rootItem;
    
    // Find or create parent items
    for (int i = 0; i < parts.size() - 1; ++i)
    {
        BoardParameterTreeItem* childItem = nullptr;
        bool found = false;
        for (int j = 0; j < parentItem->childCount(); ++j)
        {
            if (parentItem->child(j)->label() == parts[i])
            {
                childItem = parentItem->child(j);
                found = true;
                break;
            }
        }

        if (!found)
        {
            beginInsertRows(parent(createIndex(parentItem->row(), 0, parentItem)), parentItem->childCount(), parentItem->childCount());
            childItem = new BoardParameterTreeItem(parts[i], parentItem);
            parentItem->appendChild(childItem);
            endInsertRows();
        }
        parentItem = childItem;
    }


    // Find or create the parameter item itself
    const QString paramShortName = parts.last();
    BoardParameterTreeItem* paramItem = nullptr;
    int paramRow = -1;
    for (int i = 0; i < parentItem->childCount(); ++i)
    {
        BoardParameterTreeItem* child = parentItem->child(i);
        if (child->label() == paramShortName)
        {
            paramItem = child;
            paramRow = i;
            break;
        }
    }

    if (!paramItem)
    {
        paramRow = parentItem->childCount();
        beginInsertRows(parent(createIndex(parentItem->row(), 0, parentItem)), paramRow, paramRow);
        paramItem = new BoardParameterTreeItem(paramShortName, parentItem, parameter);
        parentItem->appendChild(paramItem);
        m_parameterCount++;
        emit countParametersChanged();
        m_chartVisibilities.append(false);
        endInsertRows();
    }
    else
    {
        paramItem->setParameter(parameter);
        QModelIndex itemIndex = createIndex(paramItem->row(), 0, paramItem);
        emit dataChanged(itemIndex, itemIndex, { ValueRole, TimeRole, UnitRole });
    }

    // Handle array values
    if (parameter->value().canConvert<QVariantList>())
    {
        QVariantList list = parameter->value().toList();

        // Clear old array elements
        if (paramItem->childCount() > 0)
        {
            beginRemoveRows(createIndex(paramRow, 0, paramItem), 0, paramItem->childCount() - 1);
            qDeleteAll(paramItem->children());
            paramItem->clearChildren();
            endRemoveRows();
        }

        // Add new array elements
        if (!list.isEmpty())
        {
            beginInsertRows(createIndex(paramRow, 0, paramItem), 0, list.count() - 1);
            for (int i = 0; i < list.count(); ++i)
            {
                BoardParameterTreeItem* arrayElementItem = new BoardParameterTreeItem(QString("[%1]").arg(i), paramItem, nullptr, list.at(i));
                paramItem->appendChild(arrayElementItem);
            }
            endInsertRows();
        }
    }
}

void BoardParametersTreeModel::setPlayer(DataPlayer* dataPlayer)
{
    m_dataPlayer = dataPlayer;

    if (m_playConnection)
    {
        QObject::disconnect(m_playConnection);
    }

    if (m_stopConnection)
    {
        QObject::disconnect(m_stopConnection);
    }

    clearParameters();

    m_playConnection = connect(m_dataPlayer, &DataPlayer::parameterPlayed,
        this, &BoardParametersTreeModel::onNewParameterAdded);
}

QModelIndex BoardParametersTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    BoardParameterTreeItem* parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());

    BoardParameterTreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex BoardParametersTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    BoardParameterTreeItem* childItem = static_cast<BoardParameterTreeItem*>(index.internalPointer());
    BoardParameterTreeItem* parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int BoardParametersTreeModel::rowCount(const QModelIndex& parent) const
{
    auto count = m_topLevelItems.count();

    return 1;

    return m_topLevelItems.count();

    BoardParameterTreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<BoardParameterTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int BoardParametersTreeModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant BoardParametersTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    BoardParameterTreeItem* item = static_cast<BoardParameterTreeItem*>(index.internalPointer());
    BoardParameterSingle* parameter = item->parameter();

    switch (static_cast<ParameterRole>(role))
    {
    case ParameterRole::LabelRole:
        return item->label();
    case ParameterRole::ValueRole:
    {
        if (item->itemValue().isValid())
        {
            return item->itemValue();
        }
        if (!parameter) return {};
        bool ok;
        auto doubleValue = parameter->value().toDouble(&ok);
        if (!ok) return parameter->value();
        return QString::number(doubleValue, 'g', 3);
    }
    case ParameterRole::UnitRole:
        return parameter ? QVariant::fromValue(parameter->unit()) : QVariant();
    case ParameterRole::TimeRole:
        return parameter ? QVariant::fromValue(parameter->timestamp()) : QVariant();
    case ParameterRole::ChartVisibilityRole:
        return QVariant::fromValue(m_chartVisibilities.value(index.row())); // This is simplistic and might need adjustment for tree
    case ParameterRole::ColorRole:
        return QVariant::fromValue(m_colors.value(index.row())); // This is simplistic and might need adjustment for tree
    case ParameterRole::FullPathRole:
        return item->fullPath();
    }

    return QVariant();
}

bool BoardParametersTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()) return false;

    if (role == (int)ParameterRole::ChartVisibilityRole)
    {
        // This is simplistic and might need adjustment for tree
        m_chartVisibilities[index.row()] = value.toBool();
        emit dataChanged(index, index, { role });
        return true;
    }

    return false;
}

QHash<int, QByteArray> BoardParametersTreeModel::roleNames() const
{
    QHash<int, QByteArray> rolesHash;
    rolesHash[(int)ParameterRole::LabelRole] = "label";
    rolesHash[(int)ParameterRole::ValueRole] = "value";
    rolesHash[(int)ParameterRole::UnitRole] = "unit";
    rolesHash[(int)ParameterRole::TimeRole] = "timestamp";
    rolesHash[(int)ParameterRole::ChartVisibilityRole] = "chartVisibility";
    rolesHash[(int)ParameterRole::ColorRole] = "parameterColor";
    rolesHash[(int)ParameterRole::FullPathRole] = "fullPath";
    return rolesHash;
}

int BoardParametersTreeModel::getCountParameters() const
{
    return m_parameterCount;
}

void BoardParametersTreeModel::clearParameters()
{
    onParametersCleared();
}

void BoardParametersTreeModel::onParametersCleared()
{
    beginResetModel();
    delete m_rootItem;
    m_rootItem = new BoardParameterTreeItem("root");
    m_topLevelItems.clear();
    m_chartVisibilities.clear();
    m_parameterCount = 0;
    endResetModel();
    emit countParametersChanged();
}

void BoardParametersTreeModel::makeRandomColors()
{
    int hue = 0;
    int hueStep = 60 + 5;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib_int(192, 255);
    for (auto i = 0; i < 200; i++)
    {
        auto sat = distrib_int(gen);
        auto val = distrib_int(gen);
        m_colors.append(QColor::fromHsv(hue, sat, val, 255));
        hue = (hue + hueStep) % 360;
    }
}

BoardParametersTreeModel::~BoardParametersTreeModel()
{
    delete m_rootItem;
}
