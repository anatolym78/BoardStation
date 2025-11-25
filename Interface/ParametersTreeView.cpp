#include "ParametersTreeView.h"
#include "ValueColumnDelegate.h"

#include <QHeaderView>
#include <QMouseEvent>

#include "./../Model/Parameters/Tree/ParameterTreeHistoryItem.h"

ParametersTreeView::ParametersTreeView(QWidget *parent)
	: QTreeView(parent)
{
	setAlternatingRowColors(false);
	setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(true);
	setItemsExpandable(true);
	setAllColumnsShowFocus(true);
	setUniformRowHeights(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setExpandsOnDoubleClick(true);

	// Настройка заголовка
	header()->setStretchLastSection(true);

	setMouseTracking(true);
}

void ParametersTreeView::setModel(QAbstractItemModel* model)
{
	QTreeView::setModel(model);

	if (!model)
		return;
	
	model->setHeaderData(0, Qt::Horizontal, tr("label"));
	model->setHeaderData(1, Qt::Horizontal, tr("value"));

	header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	header()->setSectionResizeMode(1, QHeaderView::Stretch);

	// Устанавливаем делегат для колонки значений (индекс 1)
	setItemDelegateForColumn(1, new ValueColumnDelegate(this));
}

void ParametersTreeView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());

	if (index.isValid())
	{
		auto treeItem = static_cast<ParameterTreeHistoryItem*>(index.internalPointer());

		if (treeItem && treeItem->type() == ParameterTreeItem::ItemType::History)
		{
			emit itemHovered(treeItem);
		}
	}

	QTreeView::mouseMoveEvent(event);
}

void ParametersTreeView::leaveEvent(QEvent* event)
{
	emit itemHovered(nullptr); // Отправляем пустой индекс

	QTreeView::leaveEvent(event);
}
