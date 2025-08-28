#include "EngineSliderDelegate.h"
#include "EnginesModel.h"
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QSlider>

EngineSliderDelegate::EngineSliderDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* EngineSliderDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    
    // Создаем слайдер только для третьей колонки (слайдеры)
    if (index.column() == 2) {
        QSlider *slider = createSlider(parent);
        
        // Устанавливаем значение из модели (из колонки значения)
        QModelIndex valueIndex = index.model()->index(index.row(), 1);
        int value = valueIndex.data().toInt();
        slider->setValue(value);
        
        return slider;
    }
    
    return nullptr;
}

void EngineSliderDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 2 && editor) {
        QSlider *slider = qobject_cast<QSlider*>(editor);
        if (slider) {
            // Получаем значение из модели (из колонки значения, а не из колонки слайдера)
            QModelIndex valueIndex = index.model()->index(index.row(), 1);
            int value = valueIndex.data().toInt();
            slider->setValue(value);
        }
    }
}

void EngineSliderDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 2 && editor) {
        QSlider *slider = qobject_cast<QSlider*>(editor);
        if (slider) {
            // Получаем новое значение от слайдера
            int value = slider->value();
            
            // Обновляем значение в колонке значения (колонка 1)
            QModelIndex valueIndex = model->index(index.row(), 1);
            model->setData(valueIndex, value, Qt::DisplayRole);
            
            // Уведомляем модель об изменении данных
            model->setData(index, value, Qt::EditRole);
        }
    }
}

void EngineSliderDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    
    if (editor) {
        // Центрируем слайдер по вертикали в ячейке
        QRect rect = option.rect;
        int sliderHeight = editor->sizeHint().height();
        int y = rect.y() + (rect.height() - sliderHeight) / 2;
        
        editor->setGeometry(rect.x(), y, rect.width(), sliderHeight);
    }
}

void EngineSliderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 2) {
        // Для колонки со слайдером рисуем только фон
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        // Рисуем фон
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
        
        // Не рисуем никакого текста - слайдер будет отображаться через редактор
    } else {
        // Для остальных колонок используем стандартное отображение
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize EngineSliderDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 2) {
        // Для колонки со слайдером устанавливаем минимальный размер
        return QSize(100, 25);
    }
    
    return QStyledItemDelegate::sizeHint(option, index);
}

QSlider* EngineSliderDelegate::createSlider(QWidget *parent) const
{
    QSlider *slider = new QSlider(Qt::Horizontal, parent);
    slider->setRange(0, 100);
    slider->setMinimumWidth(80);
    slider->setMinimumHeight(16);
    slider->setMaximumHeight(20);
    
    return slider;
}
