#ifndef ENGINESLIDERDELEGATE_H
#define ENGINESLIDERDELEGATE_H

#include <QStyledItemDelegate>
#include <QSlider>

class EngineSliderDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EngineSliderDelegate(QObject *parent = nullptr);
    ~EngineSliderDelegate() override = default;

    // QStyledItemDelegate interface
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QSlider* createSlider(QWidget *parent) const;
};

#endif // ENGINESLIDERDELEGATE_H
