#include "DebugView.h"
#include <QHeaderView>
#include <QAbstractItemModel>

DebugView::DebugView(QWidget *parent) : QTableView(parent)
{
	// Настройка заголовков
	verticalHeader()->setVisible(false);
	horizontalHeader()->setStretchLastSection(false); // Отключаем растягивание последней колонки
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

void DebugView::setModel(QAbstractItemModel *model)
{
	QTableView::setModel(model);
	setupColumnWidths();
}

void DebugView::setupColumnWidths()
{
	if (model() && model()->columnCount() >= 3)
	{
		setColumnWidth(0, 120);  // Колонка "Тип"
		setColumnWidth(1, 80);  // Колонка "Сообщение"
		setColumnWidth(2, 640);  // Колонка "Время"
	}
}

