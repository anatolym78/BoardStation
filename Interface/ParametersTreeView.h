#ifndef PARAMETERSTREEVIEW_H
#define PARAMETERSTREEVIEW_H

#include <QTreeView>
#include "./../Model/Parameters/Tree/ParameterTreeHistoryItem.h"

class ParametersTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ParametersTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel* model) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

signals:
    void itemHovered(ParameterTreeHistoryItem* treeItem);
};

#endif // PARAMETERSTREEVIEW_H
