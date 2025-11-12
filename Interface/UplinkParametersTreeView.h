#ifndef UPLINKPARAMETERSTREEVIEW_H
#define UPLINKPARAMETERSTREEVIEW_H

#include <QTreeView>

class UplinkParametersTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit UplinkParametersTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel* model) override;

private:
    void setupControlWidgets();
    void setupControlWidgetsRecursive(const QModelIndex &parent = QModelIndex());
    void clearControlWidgetsRecursive(const QModelIndex &parent = QModelIndex());
    QWidget* createControlWidget(const QModelIndex &index) const;
};

#endif // UPLINKPARAMETERSTREEVIEW_H

