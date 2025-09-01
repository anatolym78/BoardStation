#include "OutParameterWidgetCreator.h"
#include "OutParameter.h"
#include "RealOutParameter.h"
#include "ListedRealOutParameter.h"
#include "RangedRealOutParameter.h"
#include "StringOutParameter.h"
#include "BooleanOutParameter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>

OutParameterWidgetCreator::OutParameterWidgetCreator(QObject *parent)
    : QObject(parent)
{
}

QWidget* OutParameterWidgetCreator::createControlWidget(OutParameter *parameter)
{
    if (!parameter) {
        qWarning() << "OutParameterWidgetCreator::createControlWidget - параметр равен nullptr";
        return nullptr;
    }
    
    QString controlType = parameter->getControlType();
    
    // Определяем тип параметра и создаем соответствующий виджет
    if (auto listedReal = dynamic_cast<ListedRealOutParameter*>(parameter)) {
        return createWidgetForListedReal(listedReal);
    } else if (auto rangedReal = dynamic_cast<RangedRealOutParameter*>(parameter)) {
        return createWidgetForRangedReal(rangedReal);
    } else if (auto stringParam = dynamic_cast<StringOutParameter*>(parameter)) {
        return createWidgetForString(stringParam);
    } else if (auto booleanParam = dynamic_cast<BooleanOutParameter*>(parameter)) {
        return createWidgetForBoolean(booleanParam);
    } else if (auto realParam = dynamic_cast<RealOutParameter*>(parameter)) {
        return createWidgetForReal(realParam);
    }
    
    qWarning() << "OutParameterWidgetCreator::createControlWidget - неизвестный тип параметра:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetForListedReal(ListedRealOutParameter *parameter)
{
    if (!parameter) return nullptr;
    
    QString controlType = parameter->getControlType();
    QList<double> values = parameter->getValues();
    
    if (controlType == "QComboBox") {
        QComboBox *comboBox = new QComboBox();
        
        // Заполняем значениями
        double currentValue = parameter->getValue();
        int currentIndex = 0;
        
        for (int i = 0; i < values.size(); ++i) {
            double value = values[i];
            comboBox->addItem(QString::number(value), value);
            
            // Запоминаем индекс текущего значения
            if (std::abs(value - currentValue) < 0.001) {
                currentIndex = i;
            }
        }
        
        comboBox->setCurrentIndex(currentIndex);
        
        // Подключаем сигнал изменения
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [parameter, comboBox](int index) {
            if (index >= 0) {
                double newValue = comboBox->itemData(index).toDouble();
                parameter->setValue(newValue);
                emit parameter->valueChanged();
            }
        });
        
        return createWidgetContainer(comboBox);
        
    } else if (controlType == "QSpinBox") {
        QSpinBox *spinBox = new QSpinBox();
        
        if (!values.isEmpty()) {
            spinBox->setRange(static_cast<int>(values.first()), 
                             static_cast<int>(values.last()));
            spinBox->setValue(static_cast<int>(parameter->getValue()));
        }
        
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [parameter](int value) {
            parameter->setValue(static_cast<double>(value));
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(spinBox);
        
    } else if (controlType == "QSlider") {
        QSlider *slider = new QSlider(Qt::Horizontal);
        
        if (!values.isEmpty()) {
            slider->setRange(static_cast<int>(values.first()), 
                            static_cast<int>(values.last()));
            slider->setValue(static_cast<int>(parameter->getValue()));
        }
        
        connect(slider, &QSlider::valueChanged,
                [parameter](int value) {
            parameter->setValue(static_cast<double>(value));
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(slider);
    }
    
    qWarning() << "OutParameterWidgetCreator::createWidgetForListedReal - неподдерживаемый тип контрола:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetForRangedReal(RangedRealOutParameter *parameter)
{
    if (!parameter) return nullptr;
    
    QString controlType = parameter->getControlType();
    
    if (controlType == "QSpinBox") {
        QSpinBox *spinBox = new QSpinBox();
        
        spinBox->setRange(static_cast<int>(parameter->getMinValue()), 
                         static_cast<int>(parameter->getMaxValue()));
        spinBox->setValue(static_cast<int>(parameter->getValue()));
        
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [parameter](int value) {
            parameter->setValue(static_cast<double>(value));
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(spinBox);
        
    } else if (controlType == "QSlider") {
        QSlider *slider = new QSlider(Qt::Horizontal);
        
        slider->setRange(static_cast<int>(parameter->getMinValue()), 
                         static_cast<int>(parameter->getMaxValue()));
        slider->setValue(static_cast<int>(parameter->getValue()));
        
        connect(slider, &QSlider::valueChanged,
                [parameter](int value) {
            parameter->setValue(static_cast<double>(value));
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(slider);
    }
    
    qWarning() << "OutParameterWidgetCreator::createWidgetForRangedReal - неподдерживаемый тип контрола:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetForString(StringOutParameter *parameter)
{
    if (!parameter) return nullptr;
    
    QString controlType = parameter->getControlType();
    
    if (controlType == "QLineEdit") {
        QLineEdit *lineEdit = new QLineEdit();
        
        lineEdit->setText(parameter->getValue());
        
        connect(lineEdit, &QLineEdit::textChanged,
                [parameter](const QString &text) {
            parameter->setValue(text);
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(lineEdit);
        
    } else if (controlType == "QTextEdit") {
        QTextEdit *textEdit = new QTextEdit();
        
        textEdit->setPlainText(parameter->getValue());
        
        // Устанавливаем максимальные размеры для центрирования
        textEdit->setMaximumHeight(60);
        textEdit->setMaximumWidth(200);
        
        connect(textEdit, &QTextEdit::textChanged,
                [parameter, textEdit]() {
            parameter->setValue(textEdit->toPlainText());
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(textEdit);
        
    } else if (controlType == "QComboBox") {
        QComboBox *comboBox = new QComboBox();
        
        QStringList values = parameter->getValues();
        comboBox->addItems(values);
        
        QString currentValue = parameter->getValue();
        int currentIndex = values.indexOf(currentValue);
        if (currentIndex >= 0) {
            comboBox->setCurrentIndex(currentIndex);
        }
        
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [parameter, comboBox](int index) {
            if (index >= 0) {
                QString newValue = comboBox->itemText(index);
                parameter->setValue(newValue);
                emit parameter->valueChanged();
            }
        });
        
        return createWidgetContainer(comboBox);
    }
    
    qWarning() << "OutParameterWidgetCreator::createWidgetForString - неподдерживаемый тип контрола:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetForBoolean(BooleanOutParameter *parameter)
{
    if (!parameter) return nullptr;
    
    QString controlType = parameter->getControlType();
    
    if (controlType == "QCheckBox") {
        QCheckBox *checkBox = new QCheckBox();
        
        checkBox->setChecked(parameter->getValue());
        
        connect(checkBox, &QCheckBox::toggled,
                [parameter](bool checked) {
            parameter->setValue(checked);
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(checkBox);
        
    } else if (controlType == "QComboBox") {
        QComboBox *comboBox = new QComboBox();
        
        comboBox->addItem("False", false);
        comboBox->addItem("True", true);
        
        comboBox->setCurrentIndex(parameter->getValue() ? 1 : 0);
        
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [parameter, comboBox](int index) {
            if (index >= 0) {
                bool newValue = comboBox->itemData(index).toBool();
                parameter->setValue(newValue);
                emit parameter->valueChanged();
            }
        });
        
        return createWidgetContainer(comboBox);
    }
    
    qWarning() << "OutParameterWidgetCreator::createWidgetForBoolean - неподдерживаемый тип контрола:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetForReal(RealOutParameter *parameter)
{
    if (!parameter) return nullptr;
    
    QString controlType = parameter->getControlType();
    
    if (controlType == "QSpinBox") {
        QSpinBox *spinBox = new QSpinBox();
        
        spinBox->setRange(-999999, 999999);
        spinBox->setValue(static_cast<int>(parameter->getValue()));
        
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [parameter](int value) {
            parameter->setValue(static_cast<double>(value));
            emit parameter->valueChanged();
        });
        
        return createWidgetContainer(spinBox);
        
    } else if (controlType == "QLineEdit") {
        QLineEdit *lineEdit = new QLineEdit();
        
        lineEdit->setText(QString::number(parameter->getValue()));
        
        connect(lineEdit, &QLineEdit::textChanged,
                [parameter](const QString &text) {
            bool ok;
            double newValue = text.toDouble(&ok);
            if (ok) {
                parameter->setValue(newValue);
                emit parameter->valueChanged();
            }
        });
        
        return createWidgetContainer(lineEdit);
    }
    
    qWarning() << "OutParameterWidgetCreator::createWidgetForReal - неподдерживаемый тип контрола:" << controlType;
    return nullptr;
}

QWidget* OutParameterWidgetCreator::createWidgetContainer(QWidget *widget)
{
    if (!widget) return nullptr;
    
    // Создаем контейнер с отступами и выравниванием
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);
    
    layout->addWidget(widget);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    
    return container;
}
