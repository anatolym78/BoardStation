#include "ParametersTreeView.h"

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
}

void ParametersTreeView::mouseMoveEvent(QMouseEvent* event)
{
	// 1. Получаем индекс элемента по текущим координатам мыши
	QModelIndex index = indexAt(event->pos());

	// 2. Проверяем, действительно ли курсор находится над каким-то элементом (а не на пустом месте)
	if (index.isValid())
	{
		// --- Что делать дальше: ---

		// A) Если вам нужно просто подсветить этот элемент внутри этого же TreeView (как стандартное поведение Windows/macOS):
		// Используйте метод setCurrentIndex(), чтобы выделить элемент.
		//setCurrentIndex(index);

		// B) Если вам нужно сообщить ДРУГОМУ виджету об этом индексе (как в вашем предыдущем вопросе с двумя списками):
		// Вам нужно создать пользовательский сигнал в вашем классе.
		emit itemHovered(index);
	}
	else
	{
		// Курсор ушел с элементов списка, можно сбросить подсветку
		// setCurrentIndex(QModelIndex()); // Сброс выделения
		emit itemHovered(QModelIndex()); // Отправляем пустой индекс
	}

	// Обязательно вызываем базовый класс, чтобы сохранить стандартное поведение QTreeView
	QTreeView::mouseMoveEvent(event);
}
