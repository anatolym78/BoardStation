#ifndef SIMPLETREEMODEL_H
#define SIMPLETREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class SimpleTreeItem;

class SimpleTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SimpleTreeModel(QObject *parent = nullptr);
    ~SimpleTreeModel();

    // --- ОБЯЗАТЕЛЬНЫЕ ВИРТУАЛЬНЫЕ ФУНКЦИИ ДЛЯ ПЕРЕОПРЕДЕЛЕНИЯ ---

    // Возвращает QModelIndex для элемента по родителю, строке и колонке
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    // Возвращает родителя для заданного QModelIndex
    QModelIndex parent(const QModelIndex &index) const override;

    // Возвращает количество строк (дочерних элементов) у родителя
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Возвращает количество колонок у родителя
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Возвращает данные для заданного QModelIndex и роли
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // --- КОНЕЦ ОБЯЗАТЕЛЬНЫХ ФУНКЦИЙ ---


    // (Опционально) Для редактируемых моделей
    // Qt::ItemFlags flags(const QModelIndex &index) const override;
    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // (Опционально) Для заголовков
    // QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void setupModelData(const QStringList &lines, SimpleTreeItem *parent);
    SimpleTreeItem *rootItem;
};

#endif // SIMPLETREEMODEL_H
