#include "UplinkParametersModel.h"
#include <QDebug>
#include <QTableView>
#include "Model/Parameters/IntUplinkParameter.h"
#include "Model/Parameters/DoubleUplinkParameter.h"
#include "Model/Parameters/StringUplinkParameter.h"
#include "Model/Parameters/BoolUplinkParameter.h"

UplinkParametersModel::UplinkParametersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // Создаем таймер для отслеживания завершения инициализации
    m_initializationTimer = new QTimer(this);
    m_initializationTimer->setSingleShot(true);
    m_initializationTimer->setInterval(1000); // 1 секунда задержки
    connect(m_initializationTimer, &QTimer::timeout, this, &UplinkParametersModel::onInitializationTimeout);
    
    setupParameters();
}

int UplinkParametersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_parameters.size();
}

int UplinkParametersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3; // Три колонки: название, значение, управление
}

QHash<int, QByteArray> UplinkParametersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[(int)UplinkParameterRole::LabelRole] = "parameterLabel";
    roles[(int)UplinkParameterRole::ValueRole] = "parameterValue";
    roles[(int)UplinkParameterRole::DataControlRole] = "dataControl";
    roles[(int)UplinkParameterRole::ValueTypeRole] = "valueType";
    return roles;
}

QVariantMap UplinkParametersModel::createParameterData(BasicUplinkParameter* parameter) const
{
    QVariantMap paramData;

    if (auto intParameter = dynamic_cast<IntUplinkParameter*>(parameter))
    {
        paramData["controlType"] = intParameter->getControlType();
        paramData["valueType"] = "int";

        if (intParameter->hasRange())
        {
            QVariantList variantRange;
            variantRange.append(intParameter->getMinimum());
            variantRange.append(intParameter->getMaximum());
            paramData["range"] = variantRange;
        }

        if (intParameter->hasStep())
        {
            paramData["step"] = intParameter->getStep();
        }

        if (intParameter->isListParameter())
        {
            QVariantList values;
            for (const QVariant &value : intParameter->getValues())
            {
                values << value.toInt();
            }
            paramData["values"] = values;
        }

        paramData["currentValue"] = intParameter->getTypedValue();

        return paramData;
    }

    if (auto doubleParameter = dynamic_cast<DoubleUplinkParameter*>(parameter))
    {
        paramData["controlType"] = doubleParameter->getControlType();
        paramData["valueType"] = "double";

        if (doubleParameter->hasRange())
        {
            QVariantList variantRange;
            variantRange.append(doubleParameter->getMinimum());
            variantRange.append(doubleParameter->getMaximum());
            paramData["range"] = variantRange;
        }

        if (doubleParameter->hasStep())
        {
            paramData["step"] = doubleParameter->getStep();
        }

        if (doubleParameter->isListParameter())
        {
            QVariantList values;
            for (const QVariant &value : doubleParameter->getValues())
            {
                values << value.toDouble();
            }
            paramData["values"] = values;
        }

        paramData["currentValue"] = doubleParameter->getTypedValue();
        return paramData;
    }

    if (auto stringParameter = dynamic_cast<StringUplinkParameter*>(parameter))
    {
        paramData["controlType"] = stringParameter->getControlType();
        paramData["valueType"] = "string";

        if (stringParameter->isListParameter())
        {
            QVariantList values;
            for (const QVariant &value : stringParameter->getValues())
            {
                values << value.toString();
            }
            paramData["values"] = values;
        }

        paramData["currentValue"] = stringParameter->getTypedValue();
        return paramData;
    }

    if (auto boolParameter = dynamic_cast<BoolUplinkParameter*>(parameter))
    {
        paramData["controlType"] = boolParameter->getControlType();
        paramData["valueType"] = "bool";
        paramData["currentValue"] = boolParameter->getTypedValue();
        paramData["valueAlias"] = boolParameter->getValueAlias();
        paramData["falseAlias"] = boolParameter->getFalseAlias();
        paramData["trueAlias"] = boolParameter->getTrueAlias();

        if (boolParameter->isListParameter())
        {
            QVariantList values;
            for (const QVariant &value : boolParameter->getValues())
            {
                values << value.toBool();
            }
            paramData["values"] = values;
        }

        return paramData;
    }

    return {};
}

QVariant UplinkParametersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_parameters.size()) 
    {
        return QVariant();
    }

    auto row = index.row();
    auto parameter = m_parameters[row];
    auto uplinkParameterRole = (UplinkParameterRole)role;

    switch (uplinkParameterRole)
    {
    case UplinkParameterRole::LabelRole:
        return parameter->getLabel();
    case UplinkParameterRole::ValueRole:
        if (auto boolParam = dynamic_cast<BoolUplinkParameter*>(parameter))
        {
            return boolParam->getValueAlias();
        }
        return parameter->getValue();
    case UplinkParameterRole::DataControlRole:
        return createParameterData(parameter);
    case UplinkParameterRole::ValueTypeRole:
    {
        if(parameter->isBoolParameter()) return "bool";
        if(parameter->isIntParameter()) return "int";
        if(parameter->isDoubleParameter()) return "real";
        if(parameter->isStringParameter()) return "str";
    }

    }

    return {};
}

bool UplinkParametersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_parameters.size())
    {
        return false;
    }

    auto row = index.row();
    if (role == (int)UplinkParameterRole::ValueRole)
    {
        auto parameter = m_parameters[row];
        
        // Проверяем, изменилось ли значение
        QVariant currentValue = parameter->getValue();
        if (currentValue == value)
        {
            // Значение не изменилось, ничего не делаем
            return true;
        }
        
        parameter->setValue(value);

        QModelIndex topLeft = this->index(row, 0);
        QModelIndex bottomRight = this->index(row, columnCount() - 1);

        emit dataChanged(topLeft, bottomRight);
        
        // Проверяем, нужно ли эмитировать сигнал для автоматической отправки
        QString controlType = parameter->getControlType();
        if (controlType == "CheckBox" || controlType == "Switch" || controlType == "SpinBox" ||
            controlType == "Slider" || controlType == "ComboBox")
        {
            // Не отправляем сигнал во время инициализации
            if (!m_isInitializing)
            {
                emit parameterChanged(parameter);
            }
        }
    }

    return true;
}

void UplinkParametersModel::setParameterValueSilently(int row, const QVariant &value)
{
    if (row < 0 || row >= m_parameters.size())
    {
        return;
    }
    
    auto parameter = m_parameters[row];
    
    // Проверяем, изменилось ли значение
    QVariant currentValue = parameter->getValue();
    if (currentValue == value)
    {
        // Значение не изменилось, ничего не делаем
        return;
    }
    
    parameter->setValue(value);
    
    // Обновляем UI без отправки сигнала parameterChanged
    QModelIndex topLeft = this->index(row, 0);
    QModelIndex bottomRight = this->index(row, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
}

QVariant UplinkParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Название");
        case 1:
            return tr("Значение");
        case 2:
            return tr("Управление");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags UplinkParametersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    // Разрешаем редактирование только для третьей колонки (управление)
    if (index.column() == 2)
    {
        flags |= Qt::ItemIsEditable;
    }
    
    return flags;
}

void UplinkParametersModel::setupParameters()
{
    // Пока ничего не делаем
}

void UplinkParametersModel::setParameters(const QList<BasicUplinkParameter*> &parameters)
{
    m_isInitializing = true; // Устанавливаем флаг инициализации
    
    beginResetModel();
    m_parameters = parameters;
    endResetModel();
    
    // Запускаем таймер для завершения инициализации
    m_initializationTimer->start();
}

void UplinkParametersModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}

void UplinkParametersModel::onInitializationTimeout()
{
    m_isInitializing = false; // Сбрасываем флаг инициализации
    //qDebug() << "UplinkParametersModel: Initialization completed, auto-send enabled";
}
