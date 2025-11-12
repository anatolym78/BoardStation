#include "UplinkParametersTreeView.h"
#include "ControlDelegate.h"
#include "Model/Parameters/Tree/ParameterTreeItem.h"
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

#include <QHeaderView>
#include <QFrame>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QVariantMap>
#include <QTimer>

UplinkParametersTreeView::UplinkParametersTreeView(QWidget *parent)
	: QTreeView(parent)
{
	setAlternatingRowColors(false);
	setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(true);
	setItemsExpandable(true);
	setAllColumnsShowFocus(true);
	setUniformRowHeights(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setExpandsOnDoubleClick(true);

	// Настройка заголовка
	header()->setStretchLastSection(true);
}

void UplinkParametersTreeView::setModel(QAbstractItemModel* model)
{
	// Отключаемся от старой модели
	if (this->model())
	{
		disconnect(this->model(), nullptr, this, nullptr);
	}

	QTreeView::setModel(model);

	if (!model)
		return;
	
	model->setHeaderData(0, Qt::Horizontal, "label");
	model->setHeaderData(1, Qt::Horizontal, "value");
	model->setHeaderData(2, Qt::Horizontal, "control");

	header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	header()->setSectionResizeMode(1, QHeaderView::Stretch);
	header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	
	// Устанавливаем делегат для третьей колонки
	setItemDelegateForColumn(2, new ControlDelegate(this));
	
	// Подключаемся к сигналам модели для пересоздания виджетов при обновлении
	connect(model, &QAbstractItemModel::modelReset, this, &UplinkParametersTreeView::setupControlWidgets);
	connect(model, &QAbstractItemModel::layoutChanged, this, &UplinkParametersTreeView::setupControlWidgets);
	
	// Создаем постоянные виджеты-контролы после установки модели
	// Используем QTimer::singleShot чтобы дождаться полной инициализации модели
	QTimer::singleShot(100, this, &UplinkParametersTreeView::setupControlWidgets);
}

void UplinkParametersTreeView::setupControlWidgets()
{
	// Очищаем все существующие виджеты
	QAbstractItemModel* model = this->model();
	if (!model)
		return;
	
	// Очищаем виджеты рекурсивно
	clearControlWidgetsRecursive();
	
	// Рекурсивно создаем виджеты для всех History элементов
	setupControlWidgetsRecursive();
}

void UplinkParametersTreeView::clearControlWidgetsRecursive(const QModelIndex &parent)
{
	QAbstractItemModel* model = this->model();
	if (!model)
		return;
	
	int rowCount = model->rowCount(parent);
	for (int row = 0; row < rowCount; ++row)
	{
		QModelIndex controlIndex = model->index(row, 2, parent);
		if (controlIndex.isValid())
		{
			QWidget* widget = indexWidget(controlIndex);
			if (widget)
			{
				setIndexWidget(controlIndex, nullptr);
				widget->deleteLater();
			}
		}
		
		QModelIndex index = model->index(row, 0, parent);
		if (index.isValid() && model->hasChildren(index))
		{
			clearControlWidgetsRecursive(index);
		}
	}
}

void UplinkParametersTreeView::setupControlWidgetsRecursive(const QModelIndex &parent)
{
	QAbstractItemModel* model = this->model();
	if (!model)
		return;
	
	int rowCount = model->rowCount(parent);
	for (int row = 0; row < rowCount; ++row)
	{
		QModelIndex index = model->index(row, 0, parent);
		if (!index.isValid())
			continue;
		
		// Проверяем, является ли элемент History элементом
		auto treeItem = static_cast<ParameterTreeItem*>(index.internalPointer());
		if (treeItem && treeItem->type() == ParameterTreeItem::ItemType::History)
		{
			// Создаем виджет-контрол для третьей колонки
			QModelIndex controlIndex = model->index(row, 2, parent);
			if (controlIndex.isValid())
			{
				QWidget* controlWidget = createControlWidget(controlIndex);
				if (controlWidget)
				{
					setIndexWidget(controlIndex, controlWidget);
				}
			}
		}
		
		// Рекурсивно обрабатываем дочерние элементы
		if (model->hasChildren(index))
		{
			setupControlWidgetsRecursive(index);
		}
	}
}

QWidget* UplinkParametersTreeView::createControlWidget(const QModelIndex &index) const
{
	if (!index.isValid())
		return nullptr;
	
	// Получаем данные о контроле через EditRole
	QVariantMap controlData = index.data(Qt::EditRole).toMap();
	QString controlType = controlData["control"].toString();
	
	if (controlType.isEmpty())
		return nullptr;
	
	QVariant minValue = controlData["min"];
	QVariant maxValue = controlData["max"];
	QVariant currentValue = controlData["value"];
	
	QWidget* widget = nullptr;
	
	if (controlType == "QSlider")
	{
		QSlider *slider = new QSlider(Qt::Horizontal, const_cast<UplinkParametersTreeView*>(this));
		if (minValue.isValid() && maxValue.isValid())
		{
			slider->setMinimum(minValue.toInt());
			slider->setMaximum(maxValue.toInt());
		}
		if (currentValue.isValid())
		{
			slider->setValue(currentValue.toInt());
		}
		widget = slider;
		
		// Подключаем сигнал изменения значения
		connect(slider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), 
				[this, index](int value) {
					if (model())
					{
						model()->setData(index, value, Qt::EditRole);
					}
				});
	}
	else if (controlType == "QSpinBox")
	{
		QSpinBox *spinBox = new QSpinBox(const_cast<UplinkParametersTreeView*>(this));
		spinBox->setFrame(false);
		if (minValue.isValid() && maxValue.isValid())
		{
			spinBox->setMinimum(minValue.toInt());
			spinBox->setMaximum(maxValue.toInt());
		}
		if (currentValue.isValid())
		{
			spinBox->setValue(currentValue.toInt());
		}
		widget = spinBox;
		
		connect(spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
				[this, index](int value) {
					if (model())
					{
						model()->setData(index, value, Qt::EditRole);
					}
				});
	}
	else if (controlType == "QDoubleSpinBox")
	{
		QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(const_cast<UplinkParametersTreeView*>(this));
		doubleSpinBox->setFrame(false);
		if (minValue.isValid() && maxValue.isValid())
		{
			doubleSpinBox->setMinimum(minValue.toDouble());
			doubleSpinBox->setMaximum(maxValue.toDouble());
		}
		if (currentValue.isValid())
		{
			doubleSpinBox->setValue(currentValue.toDouble());
		}
		widget = doubleSpinBox;
		
		connect(doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
				[this, index](double value) {
					if (model())
					{
						model()->setData(index, value, Qt::EditRole);
					}
				});
	}
	else if (controlType == "QComboBox")
	{
		QComboBox *comboBox = new QComboBox(const_cast<UplinkParametersTreeView*>(this));
		comboBox->setFrame(false);
		// Для QComboBox можно добавить логику заполнения списка значений
		widget = comboBox;
		
		connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
				[this, index, comboBox]() {
					if (model())
					{
						QVariant value = comboBox->currentData();
						if (!value.isValid())
						{
							value = comboBox->currentText();
						}
						model()->setData(index, value, Qt::EditRole);
					}
				});
	}
	
	return widget;
}

