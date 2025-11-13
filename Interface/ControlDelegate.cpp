#include "ControlDelegate.h"
#include "Model/Parameters/Tree/ParameterTreeItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
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
	
	if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = new QSpinBox(parent);
		spinBox->setFrame(false);
		return spinBox;
	}
	
	if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(parent);
		doubleSpinBox->setFrame(false);
		return doubleSpinBox;
	}
	
	if (controlType == "QComboBox")
	{
		QComboBox *comboBox = new QComboBox(parent);
		comboBox->setFrame(false);
		return comboBox;
	}
	
	if (controlType == "QCheckBox")
	{
		QCheckBox *checkBox = new QCheckBox(parent);
		return checkBox;
	}
	
	if (controlType == "QTextEdit")
	{
		QTextEdit *textEdit = new QTextEdit(parent);
		textEdit->setMaximumHeight(60);
		textEdit->setMaximumWidth(200);
		return textEdit;
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
	QString valueType = controlData["valueType"].toString();
	
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
	
	if (controlType == "QSpinBox")
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
	
	if (controlType == "QDoubleSpinBox")
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
	
	if (controlType == "QComboBox")
	{
		QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
		if (comboBox)
		{
			comboBox->clear();
			// Для QComboBox можно добавить логику заполнения списка значений
			// Пока оставляем пустым
		}
	}
	
	if (controlType == "QCheckBox")
	{
		QCheckBox *checkBox = qobject_cast<QCheckBox*>(editor);
		if (checkBox)
		{
			if (currentValue.isValid())
			{
				checkBox->setChecked(currentValue.toBool());
			}
		}
	}
	
	if (controlType == "QTextEdit")
	{
		QTextEdit *textEdit = qobject_cast<QTextEdit*>(editor);
		if (textEdit)
		{
			if (currentValue.isValid())
			{
				textEdit->setPlainText(currentValue.toString());
			}
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
	QString valueType = controlData["valueType"].toString();
	QVariant minValue = controlData["min"];
	QVariant maxValue = controlData["max"];
	
	QVariant newValue;
	
	if (controlType == "QSlider")
	{
		QSlider *slider = qobject_cast<QSlider*>(editor);
		if (slider)
		{
			newValue = slider->value();
		}
	}
	
	if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
		if (spinBox)
		{
			newValue = spinBox->value();
		}
	}
	
	if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
		if (doubleSpinBox)
		{
			newValue = doubleSpinBox->value();
		}
	}
	
	if (controlType == "QComboBox")
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
	
	if (controlType == "QCheckBox")
	{
		QCheckBox *checkBox = qobject_cast<QCheckBox*>(editor);
		if (checkBox)
		{
			newValue = checkBox->isChecked();
		}
	}
	
	if (controlType == "QTextEdit")
	{
		QTextEdit *textEdit = qobject_cast<QTextEdit*>(editor);
		if (textEdit)
		{
			QString text = textEdit->toPlainText();
			bool isValid = true;
			
			if (valueType == "int")
			{
				bool ok;
				int intValue = text.toInt(&ok);
				if (ok)
				{
					if (minValue.isValid() && maxValue.isValid())
					{
						if (intValue >= minValue.toInt() && intValue <= maxValue.toInt())
						{
							newValue = intValue;
						}
						else
						{
							isValid = false;
						}
					}
					else
					{
						newValue = intValue;
					}
				}
				else if (!text.isEmpty())
				{
					isValid = false;
				}
			}
			else if (valueType == "double")
			{
				bool ok;
				double doubleValue = text.toDouble(&ok);
				if (ok)
				{
					if (minValue.isValid() && maxValue.isValid())
					{
						if (doubleValue >= minValue.toDouble() && doubleValue <= maxValue.toDouble())
						{
							newValue = doubleValue;
						}
						else
						{
							isValid = false;
						}
					}
					else
					{
						newValue = doubleValue;
					}
				}
				else if (!text.isEmpty())
				{
					isValid = false;
				}
			}
			else
			{
				// Для строковых значений просто сохраняем текст
				newValue = text;
			}
			
			if (!isValid)
			{
				// Не сохраняем невалидное значение
				return;
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

