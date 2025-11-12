#ifndef UPLINKPARAMETERSTREEVIEW_H
#define UPLINKPARAMETERSTREEVIEW_H

#include <QTreeView>

class UplinkParametersTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit UplinkParametersTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel* model) override;
};

#endif // UPLINKPARAMETERSTREEVIEW_H

