#include "BoardParametersTreeModel.h"

#include <random>

class TreeItem
{
public:
    explicit TreeItem(QString label, TreeItem* parentItem = nullptr, BoardParameterSingle* parameter = nullptr)
        : m_label(label)
        , m_parentItem(parentItem)
        , m_parameter(parameter)
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

    BoardParameterSingle* parameter() const
    {
        return m_parameter;
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
    QString currentPath;

    for (int i = 0; i < parts.size(); ++i)
    {
        currentPath += (i > 0 ? "." : "") + parts[i];
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
            bool isLeaf = (i == parts.size() - 1);
            beginInsertRows(parent(createIndex(parentItem->row(), 0, parentItem)), parentItem->childCount(), parentItem->childCount());
            childItem = new TreeItem(parts[i], parentItem, isLeaf ? parameter : nullptr);
            parentItem->appendChild(childItem);
            if (isLeaf)
            {
                m_parameterCount++;
                emit countParametersChanged();
                m_chartVisibilities.append(false);
            }
            endInsertRows();
        }
        else if (i == parts.size() - 1) // Existing node, just update parameter
        {
            childItem->setParameter(parameter);
            QModelIndex itemIndex = createIndex(childItem->row(), 0, childItem);
            emit dataChanged(itemIndex, itemIndex);
            return;
        }

        parentItem = childItem;
    }

    QModelIndex rootIndex;
    if (rowCount(rootIndex) > 0)
    {
         emit dataChanged(index(0, 0, rootIndex), index(rowCount(rootIndex) - 1, columnCount(rootIndex) - 1, rootIndex));
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
