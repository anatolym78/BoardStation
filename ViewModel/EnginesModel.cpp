#include "EnginesModel.h"
#include <QDebug>
#include <QSlider>
#include <QHBoxLayout>
#include <QWidget>

EnginesModel::EnginesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    setupEngines();
}

int EnginesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_engines.size();
}

int EnginesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3; // Три колонки: название, значение, слайдер
}

QVariant EnginesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_engines.size())
        return QVariant();

    const EngineData &engine = m_engines.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: // Название
            return engine.name;
        case 1: // Значение
            return QString::number(engine.value);
        case 2: // Слайдер
            return QVariant(); // Слайдер не отображается как текст
        default:
            return QVariant();
        }
    }

    return QVariant();
}

bool EnginesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.column() == 2) { // Колонка со слайдером
        int row = index.row();
        if (row >= 0 && row < m_engines.size()) {
            int newValue = value.toInt();
            if (newValue >= 0 && newValue <= 100) {
                // Обновляем значение напрямую
                m_engines[row].value = newValue;
                
                // Уведомляем об изменении данных в колонке значения
                QModelIndex valueIndex = this->index(row, 1);
                emit dataChanged(valueIndex, valueIndex);
                
                return true;
            }
        }
    }

    return false;
}

QVariant EnginesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Engine");
        case 1:
            return tr("Value");
        case 2:
            return tr("Control");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags EnginesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    // Для колонки со слайдером разрешаем редактирование
    if (index.column() == 2) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void EnginesModel::setEngineValue(int row, int value)
{
    if (row < 0 || row >= m_engines.size())
        return;

    m_engines[row].value = value;
    
    // Уведомляем об изменении данных в колонке значения
    QModelIndex valueIndex = this->index(row, 1);
    emit dataChanged(valueIndex, valueIndex);
    
    // Уведомляем об изменении данных в колонке слайдера
    QModelIndex sliderIndex = this->index(row, 2);
    emit dataChanged(sliderIndex, sliderIndex);
}

int EnginesModel::getEngineValue(int row) const
{
    if (row < 0 || row >= m_engines.size())
        return 0;
    
    return m_engines[row].value;
}

QString EnginesModel::getEngineName(int row) const
{
    if (row < 0 || row >= m_engines.size())
        return QString();
    
    return m_engines[row].name;
}

void EnginesModel::setupEngines()
{
    // Создаем данные для 4 двигателей
    QList<QPair<QString, int>> engineData = {
        {"Двигатель 1", 20},
        {"Двигатель 2", 40},
        {"Двигатель 3", 50},
        {"Двигатель 4", 35}
    };

    for (const auto &data : engineData) {
        EngineData engine;
        engine.name = data.first;
        engine.value = data.second;
        engine.slider = nullptr; // Слайдеры будут создаваться через делегат
        
        m_engines.append(engine);
    }
}

QSlider* EnginesModel::getSlider(int row) const
{
    if (row < 0 || row >= m_engines.size())
        return nullptr;
    
    return m_engines[row].slider;
}
