#include "ListedRealOutParameterDelegate.h"
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QApplication>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QDebug>
#include <cmath>
#include <QPainter>
#include <QStyle>

ListedRealOutParameterDelegate::ListedRealOutParameterDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* ListedRealOutParameterDelegate::createEditor(QWidget *parent, 
                                                      const QStyleOptionViewItem &option, 
                                                      const QModelIndex &index) const
{
    // auto slider = new QSlider(parent);
    // slider->setOrientation(Qt::Horizontal);
    // slider->setMinimum(-500);
    // slider->setMaximum(500);
    // slider->setTickInterval(100);

    // return slider;

    Q_UNUSED(option)
    
    // Создаем редактор только для третьей колонки (управление)
    if (index.column() != 2) {
        return nullptr;
    }
    
    // Проверяем, что это строка с ListedRealOutParameter
    if (!isListedRealOutParameterRow(index)) {
        return nullptr;
    }
    
    // Получаем данные о параметре через UserRole
    QVariantMap paramData = index.data(Qt::UserRole).toMap();
    QString controlType = paramData["controlType"].toString();
    
    // Отладочная информация
    qDebug() << "ListedRealOutParameterDelegate::createEditor - row:" << index.row() 
             << "column:" << index.column() 
             << "controlType:" << controlType
             << "paramData keys:" << paramData.keys();
    
    if (controlType == "QComboBox") {
        QComboBox *comboBox = new QComboBox(parent);
        //comboBox->setFrame(false);
        return comboBox;
    }
    else if (controlType == "QSpinBox") {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setFrame(false);
        return spinBox;
    }
    else if (controlType == "QSlider") {
        QSlider *slider = new QSlider(Qt::Horizontal, parent);
        //slider->setFrame(false);
        return slider;
    }
    
    return nullptr;
}

void ListedRealOutParameterDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // auto v = 55;
    // QSlider *s = qobject_cast<QSlider*>(editor);

    if (!editor || index.column() != 2) {
        qDebug() << "ListedRealOutParameterDelegate::setEditorData - editor или column не подходит";
        return;
    }
    
    // Получаем данные о параметре
    QVariantMap paramData = index.data(Qt::UserRole).toMap();
    QString controlType = paramData["controlType"].toString();
    
    qDebug() << "ListedRealOutParameterDelegate::setEditorData - row:" << index.row() 
             << "controlType:" << controlType
             << "paramData keys:" << paramData.keys();
    
    // Дополнительная отладка содержимого paramData
    qDebug() << "ListedRealOutParameterDelegate::setEditorData - paramData содержимое:";
    for (auto it = paramData.begin(); it != paramData.end(); ++it) {
        qDebug() << "  " << it.key() << ":" << it.value() << "type:" << it.value().typeName();
    }
    
    if (controlType == "QComboBox") {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox) {
            qDebug() << "ListedRealOutParameterDelegate::setEditorData - создаем QComboBox";
            
            // Очищаем комбобокс
            comboBox->clear();
            
            // Получаем список значений из параметра
            QList<QVariant> values = paramData["values"].toList();
            double currentValue = paramData["currentValue"].toDouble();
            
            qDebug() << "ListedRealOutParameterDelegate::setEditorData - values count:" << values.size()
                     << "currentValue:" << currentValue;
            
            // Заполняем комбобокс значениями
            int currentIndex = 0;
            for (int i = 0; i < values.size(); ++i) {
                double value = values[i].toDouble();
                comboBox->addItem(QString::number(value), value);
                qDebug() << "ListedRealOutParameterDelegate::setEditorData - добавлен item:" << value;
                
                // Запоминаем индекс текущего значения
                if (qAbs(value - currentValue) < 0.001) {
                    currentIndex = i;
                }
            }
            
            comboBox->setCurrentIndex(currentIndex);
            qDebug() << "ListedRealOutParameterDelegate::setEditorData - установлен currentIndex:" << currentIndex
                     << "comboBox count:" << comboBox->count();
        }
    }
    else if (controlType == "QSpinBox") {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
        if (spinBox) {
            // Получаем список значений из параметра
            QList<QVariant> values = paramData["values"].toList();
            double currentValue = paramData["currentValue"].toDouble();
            
            if (!values.isEmpty()) {
                double minValue = values.first().toDouble();
                double maxValue = values.last().toDouble();
                
                spinBox->setRange(static_cast<int>(minValue), static_cast<int>(maxValue));
                spinBox->setValue(static_cast<int>(currentValue));
            }
        }
    }
    else if (controlType == "QSlider") {
        QSlider *slider = qobject_cast<QSlider*>(editor);
        if (slider) {
            // Получаем список значений из параметра
            QList<QVariant> values = paramData["values"].toList();
            double currentValue = paramData["currentValue"].toDouble();
            
            if (!values.isEmpty()) {
                double minValue = values.first().toDouble();
                double maxValue = values.last().toDouble();
                
                slider->setRange(static_cast<int>(minValue), static_cast<int>(maxValue));
                slider->setValue(static_cast<int>(currentValue));
            }
        }
    }
}

void ListedRealOutParameterDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, 
                                                  const QModelIndex &index) const
{
    if (!editor || !model || index.column() != 2) {
        return;
    }
    
    // Получаем данные о параметре
    QVariantMap paramData = index.data(Qt::UserRole).toMap();
    QString controlType = paramData["controlType"].toString();
    
    if (controlType == "QComboBox") {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox) {
            QString value = comboBox->currentText();
            auto countItems = comboBox->count();
            qDebug() << "ListedRealOutParameterDelegate::setModelData - QComboBox значение:" << value << "items count:" << countItems;
            
            // Обновляем модель
            model->setData(index, value, Qt::EditRole);
        }
    }
    else if (controlType == "QSpinBox") {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
        if (spinBox) {
            int value = spinBox->value();
            model->setData(index, QString::number(value), Qt::EditRole);
        }
    }
    else if (controlType == "QSlider") {
        QSlider *slider = qobject_cast<QSlider*>(editor);
        if (slider) {
            int value = slider->value();
            model->setData(index, QString::number(value), Qt::EditRole);
        }
    }
}

void ListedRealOutParameterDelegate::updateEditorGeometry(QWidget *editor, 
                                                         const QStyleOptionViewItem &option, 
                                                         const QModelIndex &index) const
{
    Q_UNUSED(index)
    
    if (editor) {
        editor->setGeometry(option.rect);
    }
}

bool ListedRealOutParameterDelegate::isListedRealOutParameterRow(const QModelIndex &index) const
{
    // Проверяем, что это строка с ListedRealOutParameter
    // Можно передавать эту информацию через UserRole или другой способ
    Q_UNUSED(index)
    return true; // Пока возвращаем true для всех строк
}

// void ListedRealOutParameterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
//                                            const QModelIndex &index) const
// {
//     if (index.column() != 2) {
//         // Для других колонок используем стандартную отрисовку
//         QStyledItemDelegate::paint(painter, option, index);
//         return;
//     }

    
//     // Получаем данные о параметре
//     QVariantMap paramData = index.data(Qt::UserRole).toMap();
//     QString controlType = paramData["controlType"].toString();
    
//     // Отладочная информация
//     qDebug() << "ListedRealOutParameterDelegate::paint - row:" << index.row()
//              << "column:" << index.column()
//              << "controlType:" << controlType
//              << "paramData keys:" << paramData.keys();
    
//     if (controlType.isEmpty()) {
//         // Если нет данных о контроле, используем стандартную отрисовку
//         qDebug() << "ListedRealOutParameterDelegate::paint - controlType пустой, используем стандартную отрисовку";
//         QStyledItemDelegate::paint(painter, option, index);
//         return;
//     }
    
//     // Рисуем фон
//     painter->fillRect(option.rect, option.palette.color(QPalette::Base));
    
//     // Рисуем рамку
//     painter->setPen(option.palette.color(QPalette::Text));
//     painter->drawRect(option.rect.adjusted(0, 0, -1, -1));
    
//     // Рисуем текст с типом контрола и текущим значением
//     QList<QVariant> values = paramData["values"].toList();
//     double currentValue = paramData["currentValue"].toDouble();
    
//     QString displayText;
//     if (controlType == "QComboBox") {
//         displayText = QString("Выбрать значение");
//     } else if (controlType == "QSpinBox") {
//         displayText = QString("Изменить значение");
//     } else if (controlType == "QSlider") {
//         displayText = QString("Установить значение");
//     } else {
//         displayText = QString("Контрол: %1").arg(controlType);
//     }
    
//     painter->drawText(option.rect.adjusted(4, 4, -4, -4),
//                       Qt::AlignCenter | Qt::AlignVCenter,
//                       displayText);
// }
