#ifndef PARAMETERSTREEVIEW_H
#define PARAMETERSTREEVIEW_H

#include <QTreeView>

class ParametersTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ParametersTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel* model) override;
};

#endif // PARAMETERSTREEVIEW_H
