#include "SimpleTreeModel.h"
#include "SimpleTreeItem.h"

SimpleTreeModel::SimpleTreeModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	// Создаем невидимый корневой элемент.
	// Его данные будут использоваться для заголовков колонок.
	rootItem = new SimpleTreeItem({tr("Имя"), tr("Значение")});

	// Заполняем модель тестовыми данными
	auto child1 = new SimpleTreeItem({ tr("stabData"), tr("") }, rootItem);
	rootItem->appendChild(child1);

	auto child2 = new SimpleTreeItem({ tr("throttleAdjustmentValues"), tr("[0, 0, 0, 0]") }, child1);
	child1->appendChild(child2);

	for (auto i = 0; i < 4; i++)
	{
		auto child3 = new SimpleTreeItem({ QString("index[") + QString::number(i) + QString("]"), QVariant::fromValue(0)}, child2);
		child2->appendChild(child3);
	}
}

SimpleTreeModel::~SimpleTreeModel()
{
	delete rootItem;
}

QModelIndex SimpleTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	// Если у нас нет корневого элемента или запрашиваются некорректные данные,
	// то возвращаем невалидный индекс.
	if (!rootItem || row < 0 || column < 0)
	{
		return QModelIndex();
	}

	// 1. Определяем, для какого родительского узла SimpleTreeItem мы ищем дочерний элемент.
	SimpleTreeItem* parentItem;

	if (!parent.isValid())
	{
		// Если parent невалиден, значит, представление запрашивает элемент
		// самого верхнего уровня. Его родитель - наш невидимый корень rootItem.
		parentItem = rootItem;
	}
	else
	{
		// Если parent валиден, мы можем извлечь из него указатель на наш
		// объект SimpleTreeItem, который мы сохранили там ранее.
		parentItem = static_cast<SimpleTreeItem*>(parent.internalPointer());
	}

	// 2. Запрашиваем у родительского элемента SimpleTreeItem дочерний по нужной строке.
	SimpleTreeItem* childItem = parentItem->child(row);

	// 3. Если дочерний элемент существует...
	if (childItem)
	{
		// ...создаем для него QModelIndex и возвращаем его.
		// САМОЕ ВАЖНОЕ: мы сохраняем указатель на `childItem` внутри индекса
		// с помощью `createIndex`. Это позволит нам позже (в функциях data(),
		// parent() и в следующем вызове index()) снова получить доступ
		// к нашему объекту SimpleTreeItem из QModelIndex.
		return createIndex(row, column, childItem);
	}

	// 4. Если дочернего элемента по такому `row` нет, возвращаем невалидный индекс.
	return QModelIndex();
}

QModelIndex SimpleTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return QModelIndex();
	}

	SimpleTreeItem *childItem = static_cast<SimpleTreeItem*>(index.internalPointer());
	if (!childItem)
	{
		return QModelIndex();
	}

	SimpleTreeItem *parentItem = childItem->parentItem();

	// Если родитель - это наш невидимый корень, то у элемента нет видимого родителя.
	// Возвращаем невалидный индекс.
	if (parentItem == rootItem)
	{
		return QModelIndex();
	}

	// Создаем индекс для родителя. `parentItem->row()` вернет,
	// каким по счету дочерним элементом он является у своего родителя.
	return createIndex(parentItem->row(), 0, parentItem);
}

int SimpleTreeModel::rowCount(const QModelIndex &parent) const
{
	SimpleTreeItem *parentItem;

	if (!parent.isValid())
	{
		// Если индекс-родитель невалиден, значит, запрашивают
		// количество элементов верхнего уровня.
		parentItem = rootItem;
	}
	else
	{
		parentItem = static_cast<SimpleTreeItem*>(parent.internalPointer());
	}

	// Количество дочерних элементов узла не зависит от колонки.
	// Эта функция должна возвращать одинаковое значение для всех колонок одного родителя.
	return parentItem->childCount();
}

int SimpleTreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	// Количество колонок в нашей модели постоянно.
	auto count = rootItem->columnCount();
	return rootItem->columnCount();
}

QVariant SimpleTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	SimpleTreeItem* item = static_cast<SimpleTreeItem*>(index.internalPointer());
	if (role == (int)ParameterRole::LabelRole)
	{
		return item->data(0);
	}

	if (role == (int)ParameterRole::ValueRole)
	{
		return item->data(1);
	}

	return QVariant();

	// Возвращаем данные для соответствующей колонки
	return item->data(index.column());
}

QHash<int, QByteArray> SimpleTreeModel::roleNames() const
{
	QHash<int, QByteArray> rolesHash;
	rolesHash[(int)ParameterRole::LabelRole] = "label";
	rolesHash[(int)ParameterRole::ValueRole] = "value";
	return rolesHash;
}

QVariant SimpleTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// Нам нужны только горизонтальные заголовки для отображения
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return rootItem->data(section);
	}

	return QVariant();
}
