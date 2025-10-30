#include "BoardParametersTreeModel.h"

#include <random>

class TreeItem
{
public:
    explicit TreeItem(QString label, TreeItem* parentItem = nullptr, BoardParameterSingle* parameter = nullptr, const QVariant& itemValue = QVariant())
        : m_label(label)
        , m_parentItem(parentItem)
        , m_parameter(parameter)
        , m_itemValue(itemValue)
    {
    }

    ~TreeItem()
    {
        qDeleteAll(m_childItems);
    }

    void appendChild(TreeItem* child)
    {
        m_childItems.append(child);
    }

    TreeItem* child(int row)
    {
        if (row < 0 || row >= m_childItems.size())
            return nullptr;
        return m_childItems.at(row);
    }

    int childCount() const
    {
        return m_childItems.count();
    }

    const QList<TreeItem*>& children() const { return m_childItems; }
    QList<TreeItem*>& children() { return m_childItems; }

    void clearChildren()
    {
        m_childItems.clear();
    }

    BoardParameterSingle* parameter() const
    {
        return m_parameter;
    }

    QVariant itemValue() const
    {
        return m_itemValue;
    }

    void setParameter(BoardParameterSingle* p)
    {
        m_parameter = p;
    }

    QString label() const
    {
        if (m_parameter)
        {
            auto parts = m_parameter->label().split('.');
            return parts.last();
        }
        return m_label;
    }

    TreeItem* parentItem()
    {
        return m_parentItem;
    }

    int row() const
    {
        if (m_parentItem)
            return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }

private:
    QString m_label;
    QList<TreeItem*> m_childItems;
    TreeItem* m_parentItem;
    BoardParameterSingle* m_parameter;
    QVariant m_itemValue;
};

BoardParametersTreeModel::BoardParametersTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    m_rootItem = new TreeItem("root");
    makeRandomColors();
}

BoardParametersTreeModel::~BoardParametersTreeModel()
{
    delete m_rootItem;
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

    TreeItem* parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex BoardParametersTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int BoardParametersTreeModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

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

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
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

void BoardParametersTreeModel::onNewParameterAdded(BoardParameterSingle* parameter)
{
    if (parameter == nullptr) return;

    QString label = parameter->label();
    QStringList parts = label.split('.');

    TreeItem* parentItem = m_rootItem;
    
    // Find or create parent items
    for (int i = 0; i < parts.size() - 1; ++i)
    {
        TreeItem* childItem = nullptr;
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
            childItem = new TreeItem(parts[i], parentItem);
            parentItem->appendChild(childItem);
            endInsertRows();
        }
        parentItem = childItem;
    }


    // Find or create the parameter item itself
    const QString paramShortName = parts.last();
    TreeItem* paramItem = nullptr;
    int paramRow = -1;
    for (int i = 0; i < parentItem->childCount(); ++i)
    {
        TreeItem* child = parentItem->child(i);
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
        paramItem = new TreeItem(paramShortName, parentItem, parameter);
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
                TreeItem* arrayElementItem = new TreeItem(QString("[%1]").arg(i), paramItem, nullptr, list.at(i));
                paramItem->appendChild(arrayElementItem);
            }
            endInsertRows();
        }
    }
}

void BoardParametersTreeModel::onParametersCleared()
{
    beginResetModel();
    delete m_rootItem;
    m_rootItem = new TreeItem("root");
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
