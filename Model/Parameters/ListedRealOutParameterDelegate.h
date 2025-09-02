#ifndef LISTEDREALOUTPARAMETERDELEGATE_H
#define LISTEDREALOUTPARAMETERDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

class ListedRealOutParameterDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ListedRealOutParameterDelegate(QObject *parent = nullptr);

    // QStyledItemDelegate interface
    QWidget* createEditor(QWidget *parent, 
                          const QStyleOptionViewItem &option, 
                          const QModelIndex &index) const override;
    
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, 
                      const QModelIndex &index) const override;
    
    void updateEditorGeometry(QWidget *editor, 
                             const QStyleOptionViewItem &option, 
                             const QModelIndex &index) const override;
    
    // // Переопределяем paint для постоянного отображения контролов
    // void paint(QPainter *painter, const QStyleOptionViewItem &option,
    //            const QModelIndex &index) const override;

private:
    bool isListedRealOutParameterRow(const QModelIndex &index) const;
};

#endif // LISTEDREALOUTPARAMETERDELEGATE_H
