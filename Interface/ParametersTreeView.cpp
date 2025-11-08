#include "ParametersTreeView.h"

#include <QHeaderView>

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
}

void ParametersTreeView::setModel(QAbstractItemModel* model)
{
	QTreeView::setModel(model);

	if (!model)
		return;
    
    model->setHeaderData(0, Qt::Horizontal, "label");
    model->setHeaderData(1, Qt::Horizontal, "value");

    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(1, QHeaderView::Stretch);
}
