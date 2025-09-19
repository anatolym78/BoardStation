#include "OutParametersModel.h"
#include <QDebug>
#include <QTableView>
#include "Model/Parameters/ListedRealOutParameter.h"
#include "Model/Parameters/RangedRealOutParameter.h"
#include "Model/Parameters/StringOutParameter.h"
#include "Model/Parameters/BooleanOutParameter.h"

OutParametersModel::OutParametersModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_storage(nullptr)
    , m_widgetCreator(new OutParameterWidgetCreator(this))
{
    setupParameters();
}

int OutParametersModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;

	if (m_storage == nullptr) return 0;
    
    auto countParameters = m_storage->getAllParameters().count();

    return m_storage->getAllParameters().count();

    auto rangedParameters = m_storage->getParametersOfType<RangedRealOutParameter>();

    return rangedParameters.count();
}

int OutParametersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return 3; // Три колонки: название, значение, управление
}
QHash<int, QByteArray> OutParametersModel::roleNames() const
{
	QHash<int, QByteArray> roles;
    roles[(int)OutParameterRole::LabelRole] = "parameterLabel";
    roles[(int)OutParameterRole::ValueRole] = "parameterValue";
    roles[(int)OutParameterRole::DataControlRole] = "dataControl";
	return roles;
}

QVariantMap OutParametersModel::createParameterData(OutParameter* parameter) const
{
    QVariantMap paramData;

    auto rangedParameter = dynamic_cast<RangedRealOutParameter*>(parameter);

    if(rangedParameter == nullptr) return {};

    paramData["controlType"] = rangedParameter->getControlType();

    QVariantList variantRange;
    variantRange.append(QVariant(rangedParameter->getMinValue()));
    variantRange.append(QVariant(rangedParameter->getMaxValue()));
    paramData["range"] = variantRange;

    // Добавляем шаг для SpinBox
    paramData["step"] = rangedParameter->getStep();
    paramData["currentValue"] = rangedParameter->getValue();

    return paramData;
}

QVariant OutParametersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_storage) return QVariant();

    auto row = index.row();

    auto parameters = m_storage->getAllParameters();
    auto parameter = parameters[row];

    auto outParameterRole = (OutParameterRole)role;

    switch (outParameterRole)
    {
    case OutParameterRole::LabelRole:
        return parameter->getLabel();
    case OutParameterRole::ValueRole:
        return parameter->getValueAsString();
    case OutParameterRole::DataControlRole:
        return createParameterData(parameter);
    }

    return {};

    if (role == Qt::DisplayRole)
    {
        auto parameters = m_storage->getAllParameters();
        auto parameter = parameters[row];

        if (auto rangedParameter = dynamic_cast<RangedRealOutParameter*>(parameter))
        {
			switch (index.column()) 
            {
			case 0: // Название
				return rangedParameter->getLabel();
			case 1: // Значение
				return rangedParameter->getValueAsString();
			case 2: // Управление
				return QVariant(); // Контрол будет отображаться через setIndexWidget
			default:
				return QVariant();
			}
            return QVariant();
        }

        return QVariant();

        QList<ListedRealOutParameter*> listedParams = m_storage->getParametersOfType<ListedRealOutParameter>();
        QList<RangedRealOutParameter*> rangedParams = m_storage->getParametersOfType<RangedRealOutParameter>();
        QList<StringOutParameter*> stringParams = m_storage->getParametersOfType<StringOutParameter>();
        QList<BooleanOutParameter*> booleanParams = m_storage->getParametersOfType<BooleanOutParameter>();
        
        int listedCount = listedParams.size();
        int rangedCount = rangedParams.size();
        int stringCount = stringParams.size();
        
        if (index.row() >= 0 && index.row() < listedCount)
        {
            // ListedRealOutParameter
            ListedRealOutParameter *param = listedParams[index.row()];
            
            switch (index.column())
            {
            case 0: // Название
                return param->getLabel();
            case 1: // Значение
                return param->getValueAsString();
            case 2: // Управление
                return QVariant(); // Контрол будет отображаться через setIndexWidget
            default:
                return QVariant();
            }
        }
        else
            if (index.row() >= listedCount && index.row() < listedCount + rangedCount) {
            // RangedRealOutParameter
            RangedRealOutParameter *param = rangedParams[index.row() - listedCount];
            
            switch (index.column()) {
            case 0: // Название
                return param->getLabel();
            case 1: // Значение
                return param->getValueAsString();
            case 2: // Управление
                return QVariant(); // Контрол будет отображаться через setIndexWidget
            default:
                return QVariant();
            }
        }
        else if (index.row() >= listedCount + rangedCount && index.row() < listedCount + rangedCount + stringCount) {
            // StringOutParameter
            StringOutParameter *param = stringParams[index.row() - listedCount - rangedCount];
            
            switch (index.column()) {
            case 0: // Название
                return param->getLabel();
            case 1: // Значение
                return param->getValueAsString();
            case 2: // Управление
                return QVariant(); // Контрол будет отображаться через setIndexWidget
            default:
                return QVariant();
            }
        }
        else if (index.row() >= listedCount + rangedCount + stringCount && index.row() < listedCount + rangedCount + stringCount + booleanParams.size()) {
            // BooleanOutParameter
            BooleanOutParameter *param = booleanParams[index.row() - listedCount - rangedCount - stringCount];
            
            switch (index.column()) {
            case 0: // Название
                return param->getLabel();
            case 1: // Значение
                return param->getValueAsString();
            case 2: // Управление
                return QVariant(); // Контрол будет отображаться через setIndexWidget
            default:
                return QVariant();
            }
        }
    }
    else
    {
        if (role == Qt::UserRole && index.column() == 2) 
        {
            // Передаем данные о параметре для делегата
            QList<ListedRealOutParameter*> listedParams = m_storage->getParametersOfType<ListedRealOutParameter>();
            QList<RangedRealOutParameter*> rangedParams = m_storage->getParametersOfType<RangedRealOutParameter>();
            QList<StringOutParameter*> stringParams = m_storage->getParametersOfType<StringOutParameter>();
            QList<BooleanOutParameter*> booleanParams = m_storage->getParametersOfType<BooleanOutParameter>();
        
            int listedCount = listedParams.size();
            int rangedCount = rangedParams.size();
            int stringCount = stringParams.size();
        
            if (index.row() >= 0 && index.row() < listedCount) {
                // ListedRealOutParameter
                ListedRealOutParameter *param = listedParams[index.row()];
            
                // Создаем объект с данными о параметре
                QVariantMap paramData;
                paramData["controlType"] = param->getControlType();
            
                // Преобразуем QList<double> в QVariantList напрямую
                QList<double> doubleValues = param->getValues();
                QVariantList variantValues;
                for (double value : doubleValues) {
                    variantValues.append(QVariant(value));
                }
                paramData["values"] = variantValues;
            
                paramData["currentValue"] = param->getValue();
            
                //qDebug() << "OutParametersModel::data - UserRole для ListedRealOutParameter row:" << index.row() 
                //         << "param:" << param->getLabel()
                //         << "controlType:" << param->getControlType();
            
                return paramData;
            } else if (index.row() >= listedCount && index.row() < listedCount + rangedCount) {
                // RangedRealOutParameter
                RangedRealOutParameter *param = rangedParams[index.row() - listedCount];
            
                QVariantMap paramData;
                paramData["controlType"] = param->getControlType();
            
                // Для RangedRealOutParameter используем min/max как values
                QVariantList variantValues;
                variantValues.append(QVariant(param->getMinValue()));
                variantValues.append(QVariant(param->getMaxValue()));
                paramData["values"] = variantValues;
            
                // Добавляем шаг для SpinBox
                paramData["step"] = param->getStep();
                paramData["currentValue"] = param->getValue();
            
                //qDebug() << "OutParametersModel::data - UserRole для RangedRealOutParameter row:" << index.row() 
                //         << "param:" << param->getLabel()
                //         << "controlType:" << param->getControlType();
            
                return paramData;
            } else if (index.row() >= listedCount + rangedCount && index.row() < listedCount + rangedCount + stringCount) {
                // StringOutParameter
                StringOutParameter *param = stringParams[index.row() - listedCount - rangedCount];
            
                QVariantMap paramData;
                paramData["controlType"] = param->getControlType();
                paramData["currentValue"] = param->getValue();
            
                //qDebug() << "OutParametersModel::data - UserRole для StringOutParameter row:" << index.row() 
                //         << "param:" << param->getLabel()
                //         << "controlType:" << param->getControlType();
            
                return paramData;
            } else if (index.row() >= listedCount + rangedCount + stringCount && index.row() < listedCount + rangedCount + stringCount + booleanParams.size()) {
                // BooleanOutParameter
                BooleanOutParameter *param = booleanParams[index.row() - listedCount - rangedCount - stringCount];
            
                QVariantMap paramData;
                paramData["controlType"] = param->getControlType();
                paramData["currentValue"] = param->getValue();
            
                //qDebug() << "OutParametersModel::data - UserRole для BooleanOutParameter row:" << index.row() 
                //         << "param:" << param->getLabel()
                //         << "controlType:" << param->getControlType();
            
                return paramData;
            }
        }
    }
        


    return QVariant();
}

bool OutParametersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_storage)
    {
        return false;
    }

    if(role == (int)OutParameterRole::ValueRole)
    {

    }

    return true;

    auto index2 = createIndex(0, 1);
    if(index2.isValid())
    {
        int k = 0;
    }
    auto result = setData(createIndex(0, 1), value, Qt::EditRole);

    return false;

    //if (index.column() == 1)
    //{
    //    emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));
    //    return true;
    //}
    // Обрабатываем изменения только для третьей колонки (управление)
    if(index.column() != 2) return false;
    
    QList<ListedRealOutParameter*> listedParams = m_storage->getParametersOfType<ListedRealOutParameter>();
    QList<RangedRealOutParameter*> rangedParams = m_storage->getParametersOfType<RangedRealOutParameter>();
    QList<StringOutParameter*> stringParams = m_storage->getParametersOfType<StringOutParameter>();
    QList<BooleanOutParameter*> booleanParams = m_storage->getParametersOfType<BooleanOutParameter>();

    int listedCount = listedParams.size();
    int rangedCount = rangedParams.size();
    int stringCount = stringParams.size();

    if (index.row() >= 0 && index.row() < listedCount)
    {
        // ListedRealOutParameter
        ListedRealOutParameter *param = listedParams[index.row()];

        //qDebug() << "OutParametersModel::setData - обновляем ListedRealOutParameter:" << param->getLabel()
        //         << "старое значение:" << param->getValue()
        //         << "новое значение:" << value.toString();

        // Обновляем значение параметра
        param->setValueFromString(value.toString());

        //qDebug() << "OutParametersModel::setData - параметр обновлен, новое значение:" << param->getValue();

        // Уведомляем модель об изменении данных в колонке значения (колонка 1)
        emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));

        return true;
    }
    else
        if (index.row() >= listedCount && index.row() < listedCount + rangedCount)
        {
        // RangedRealOutParameter
        RangedRealOutParameter *param = rangedParams[index.row() - listedCount];

        //qDebug() << "OutParametersModel::setData - обновляем RangedRealOutParameter:" << param->getLabel()
        //         << "старое значение:" << param->getValue()
        //         << "новое значение:" << value.toString();

        // Обновляем значение параметра
        param->setValueFromString(value.toString());

        // Уведомляем модель об изменении данных в колонке значения (колонка 1)
        emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));

        return true;
    }
    else if (index.row() >= listedCount + rangedCount && index.row() < listedCount + rangedCount + stringCount) {
        // StringOutParameter
        StringOutParameter *param = stringParams[index.row() - listedCount - rangedCount];

        //qDebug() << "OutParametersModel::setData - обновляем StringOutParameter:" << param->getLabel()
        //         << "старое значение:" << param->getValue()
        //         << "новое значение:" << value.toString();

        // Обновляем значение параметра
        param->setValueFromString(value.toString());

        //qDebug() << "OutParametersModel::setData - параметр обновлен, новое значение:" << param->getValue();

        // Уведомляем модель об изменении данных в колонке значения (колонка 1)
        emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));

        return true;
    }
    else if (index.row() >= listedCount + rangedCount + stringCount && index.row() < listedCount + rangedCount + stringCount + booleanParams.size()) {
        // BooleanOutParameter
        BooleanOutParameter *param = booleanParams[index.row() - listedCount - rangedCount - stringCount];

        //qDebug() << "OutParametersModel::setData - обновляем BooleanOutParameter:" << param->getLabel()
        //         << "старое значение:" << param->getValue()
        //         << "новое значение:" << value.toString();

        // Обновляем значение параметра
        param->setValueFromString(value.toString());

        //qDebug() << "OutParametersModel::setData - параметр обновлен, новое значение:" << param->getValue();

        // Уведомляем модель об изменении данных в колонке значения (колонка 1)
        emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));

        return true;
    }
    
    return false;
}

QVariant OutParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags OutParametersModel::flags(const QModelIndex &index) const
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

void OutParametersModel::setupParameters()
{
    // Пока ничего не делаем
}

void OutParametersModel::setStorage(OutParametersStorage *storage)
{
    if (m_storage != storage)
    {
        m_storage = storage;
        refreshModel();
    }
}

void OutParametersModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}

void OutParametersModel::createControlWidgets(QTableView *tableView)
{
    if (!m_storage || !tableView || !m_widgetCreator) return;
    
    QList<ListedRealOutParameter*> listedParams = m_storage->getParametersOfType<ListedRealOutParameter>();
    QList<RangedRealOutParameter*> rangedParams = m_storage->getParametersOfType<RangedRealOutParameter>();
    QList<StringOutParameter*> stringParams = m_storage->getParametersOfType<StringOutParameter>();
    QList<BooleanOutParameter*> booleanParams = m_storage->getParametersOfType<BooleanOutParameter>();
    
    int rowIndex = 0;
    
    // Создаем виджеты для ListedRealOutParameter
    for (int i = 0; i < listedParams.size(); ++i) {
        ListedRealOutParameter *param = listedParams[i];
        QWidget *controlWidget = m_widgetCreator->createControlWidget(param);
        
        if (controlWidget) {
            // Размещаем контейнер в третьей колонке
            tableView->setIndexWidget(index(rowIndex, 2), controlWidget);
            
            // Подключаем сигнал изменения значения к обновлению модели
            connect(param, &OutParameter::valueChanged, this, [this, rowIndex]() {
                // Уведомляем модель об изменении данных в колонке значения (колонка 1)
                emit dataChanged(this->index(rowIndex, 1), this->index(rowIndex, 1));
            });
            
            //qDebug() << "OutParametersModel::createControlWidgets - создан виджет для ListedRealOutParameter, строка" << rowIndex 
            //         << "параметр:" << param->getLabel();
        }
        rowIndex++;
    }
    
    // Создаем виджеты для RangedRealOutParameter
    for (int i = 0; i < rangedParams.size(); ++i)
    {
        RangedRealOutParameter *param = rangedParams[i];
        QWidget *controlWidget = m_widgetCreator->createControlWidget(param);
        
        if (controlWidget)
        {
            // Размещаем контейнер в третьей колонке
            tableView->setIndexWidget(index(rowIndex, 2), controlWidget);
            
            // Подключаем сигнал изменения значения к обновлению модели
            connect(param, &OutParameter::valueChanged, this, [this, rowIndex]() {
                // Уведомляем модель об изменении данных в колонке значения (колонка 1)
                emit dataChanged(this->index(rowIndex, 1), this->index(rowIndex, 1));
            });
            
            //qDebug() << "OutParametersModel::createControlWidgets - создан виджет для RangedRealOutParameter, строка" << rowIndex 
            //         << "параметр:" << param->getLabel();
        }
        rowIndex++;
    }
    
    // Создаем виджеты для StringOutParameter
    for (int i = 0; i < stringParams.size(); ++i)
    {
        StringOutParameter *param = stringParams[i];
        QWidget *controlWidget = m_widgetCreator->createControlWidget(param);
        
        if (controlWidget) {
            // Размещаем контейнер в третьей колонке
            tableView->setIndexWidget(index(rowIndex, 2), controlWidget);
            
            // Подключаем сигнал изменения значения к обновлению модели
            connect(param, &OutParameter::valueChanged, this, [this, rowIndex]() {
                // Уведомляем модель об изменении данных в колонке значения (колонка 1)
                emit dataChanged(this->index(rowIndex, 1), this->index(rowIndex, 1));
            });
            
            //qDebug() << "OutParametersModel::createControlWidgets - создан виджет для StringOutParameter, строка" << rowIndex 
            //         << "параметр:" << param->getLabel();
        }
        rowIndex++;
    }
    
    // Создаем виджеты для BooleanOutParameter
    for (int i = 0; i < booleanParams.size(); ++i)
    {
        BooleanOutParameter *param = booleanParams[i];
        QWidget *controlWidget = m_widgetCreator->createControlWidget(param);
        
        if (controlWidget) {

            // Размещаем контейнер в третьей колонке
            tableView->setIndexWidget(index(rowIndex, 2), controlWidget);
            
            // Подключаем сигнал изменения значения к обновлению модели
            connect(param, &OutParameter::valueChanged, this, [this, rowIndex]()
                    {
                // Уведомляем модель об изменении данных в колонке значения (колонка 1)
                emit dataChanged(this->index(rowIndex, 1), this->index(rowIndex, 1));
            });
            
            //qDebug() << "OutParametersModel::createControlWidgets - создан виджет для BooleanOutParameter, строка" << rowIndex 
            //         << "параметр:" << param->getLabel();
        }
        rowIndex++;
    }
}


