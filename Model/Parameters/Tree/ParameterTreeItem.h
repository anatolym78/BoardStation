#ifndef PARAMETERTREEITEM_H
#define PARAMETERTREEITEM_H

#include <QObject>
#include <QList>
#include <QString>
#include <QVariant>

class ParameterTreeItem : public QObject
{
    Q_OBJECT

public:
    explicit ParameterTreeItem(const QString &label, ParameterTreeItem *parent = nullptr);
    ~ParameterTreeItem() override;

    void appendChild(ParameterTreeItem *child);

    ParameterTreeItem *child(int row);
    ParameterTreeItem* findChildByLabel(const QString& label, bool recursive = false);
    int childCount() const;
    int row() const;
    ParameterTreeItem *parentItem() const;
    int level() const;

    QString label() const;
    QList<ParameterTreeItem*> children() const;

    enum class ItemType {
        Root,
        Group,
        History
    };
    virtual ItemType type() const = 0;

private:
    Q_DISABLE_COPY(ParameterTreeItem)

protected:
    QString m_label;
    ParameterTreeItem *m_parentItem;
    QList<ParameterTreeItem*> m_childItems;
};

#endif // PARAMETERTREEITEM_H
