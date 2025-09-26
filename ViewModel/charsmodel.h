#ifndef CHARSMODEL_H
#define CHARSMODEL_H

#include <QAbstractListModel>
#include <QObject>

class CharsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CharsModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // CHARSMODEL_H
