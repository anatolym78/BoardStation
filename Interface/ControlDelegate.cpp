#include "ControlDelegate.h"
#include "Model/Parameters/Tree/ParameterTreeItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDebug>

ControlDelegate::ControlDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

QWidget* ControlDelegate::createEditor(QWidget* parent,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	Q_UNUSED(option)
	
	// Создаем редактор только для третьей колонки (управление)
	if (index.column() != 2)
	{
		return nullptr;
	}
	
	// Проверяем, что это строка с History элементом
	if (!isHistoryItem(index))
	{
		return nullptr;
	}
	
	// Получаем данные о контроле через EditRole
	QVariantMap controlData = index.data(Qt::EditRole).toMap();
	QString controlType = controlData["control"].toString();
	
	qDebug() << "ControlDelegate::createEditor - controlType:" << controlType;
	
	if (controlType == "QSlider")
	{
		QSlider *slider = new QSlider(Qt::Horizontal, parent);
		return slider;
	}
	else if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = new QSpinBox(parent);
		spinBox->setFrame(false);
		return spinBox;
	}
	else if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(parent);
		doubleSpinBox->setFrame(false);
		return doubleSpinBox;
	}
	else if (controlType == "QComboBox")
	{
		QComboBox *comboBox = new QComboBox(parent);
		comboBox->setFrame(false);
		return comboBox;
	}
	
	return nullptr;
}

void ControlDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if (!editor || index.column() != 2)
	{
		return;
	}
	
	// Получаем данные о контроле
	QVariantMap controlData = index.data(Qt::EditRole).toMap();
	QString controlType = controlData["control"].toString();
	QVariant minValue = controlData["min"];
	QVariant maxValue = controlData["max"];
	QVariant currentValue = controlData["value"];
	
	qDebug() << "ControlDelegate::setEditorData - controlType:" << controlType
			 << "min:" << minValue << "max:" << maxValue << "value:" << currentValue;
	
	if (controlType == "QSlider")
	{
		QSlider *slider = qobject_cast<QSlider*>(editor);
		if (slider)
		{
			if (minValue.isValid() && maxValue.isValid())
			{
				slider->setMinimum(minValue.toInt());
				slider->setMaximum(maxValue.toInt());
			}
			if (currentValue.isValid())
			{
				slider->setValue(currentValue.toInt());
			}
		}
	}
	else if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
		if (spinBox)
		{
			if (minValue.isValid() && maxValue.isValid())
			{
				spinBox->setMinimum(minValue.toInt());
				spinBox->setMaximum(maxValue.toInt());
			}
			if (currentValue.isValid())
			{
				spinBox->setValue(currentValue.toInt());
			}
		}
	}
	else if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
		if (doubleSpinBox)
		{
			if (minValue.isValid() && maxValue.isValid())
			{
				doubleSpinBox->setMinimum(minValue.toDouble());
				doubleSpinBox->setMaximum(maxValue.toDouble());
			}
			if (currentValue.isValid())
			{
				doubleSpinBox->setValue(currentValue.toDouble());
			}
		}
	}
	else if (controlType == "QComboBox")
	{
		QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
		if (comboBox)
		{
			comboBox->clear();
			// Для QComboBox можно добавить логику заполнения списка значений
			// Пока оставляем пустым
		}
	}
}

void ControlDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, 
								   const QModelIndex &index) const
{
	if (!editor || !model || index.column() != 2)
	{
		return;
	}
	
	// Получаем данные о контроле
	QVariantMap controlData = index.data(Qt::EditRole).toMap();
	QString controlType = controlData["control"].toString();
	
	QVariant newValue;
	
	if (controlType == "QSlider")
	{
		QSlider *slider = qobject_cast<QSlider*>(editor);
		if (slider)
		{
			newValue = slider->value();
		}
	}
	else if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
		if (spinBox)
		{
			newValue = spinBox->value();
		}
	}
	else if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
		if (doubleSpinBox)
		{
			newValue = doubleSpinBox->value();
		}
	}
	else if (controlType == "QComboBox")
	{
		QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
		if (comboBox)
		{
			newValue = comboBox->currentData();
			if (!newValue.isValid())
			{
				newValue = comboBox->currentText();
			}
		}
	}
	
	if (newValue.isValid())
	{
		model->setData(index, newValue, Qt::EditRole);
	}
}

void ControlDelegate::updateEditorGeometry(QWidget *editor, 
										  const QStyleOptionViewItem &option, 
										  const QModelIndex &index) const
{
	Q_UNUSED(index)
	
	if (editor)
	{
		editor->setGeometry(option.rect);
	}
}

bool ControlDelegate::isHistoryItem(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return false;
	}
	
	// internalPointer() работает для любой колонки, так как указывает на один и тот же элемент
	auto treeItem = static_cast<ParameterTreeItem*>(index.internalPointer());
	if (treeItem && treeItem->type() == ParameterTreeItem::ItemType::History)
	{
		return true;
	}
	
	return false;
}

