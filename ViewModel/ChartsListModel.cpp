#include "ChartsListModel.h"
#include <QDebug>

ChartsListModel::ChartsListModel(BoardParametersStorage *parametersStorage, QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(parametersStorage)
{
    if (m_parametersStorage) {
        connect(m_parametersStorage, &BoardParametersStorage::parameterAdded, 
                this, &ChartsListModel::onParameterAdded);
    }
    
    qDebug() << "ChartsListModel: Created with parameters storage";
}

ChartsListModel::ChartsListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parametersStorage(nullptr)
{
    qDebug() << "ChartsListModel: Created empty model";
}

int ChartsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_chartsModels.size();
}

QVariant ChartsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_chartsModels.size())
        return QVariant();

    ChartSeriesModel *seriesModel = m_chartsModels.at(index.row());
    if (!seriesModel)
        return QVariant();

    switch (role) {
    case ChartNameRole:
        return m_chartNames.at(index.row());
    case SeriesModelRole:
        return QVariant::fromValue(seriesModel);
    case SeriesCountRole:
        return seriesModel->seriesCount();
    case HasSeriesRole:
        return seriesModel->seriesCount() > 0;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChartsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ChartNameRole] = "chartName";
    roles[SeriesModelRole] = "seriesModel";
    roles[SeriesCountRole] = "seriesCount";
    roles[HasSeriesRole] = "hasSeries";
    return roles;
}

void ChartsListModel::addChart(const QString &chartName)
{
    if (hasChart(chartName)) {
        qDebug() << "ChartsListModel: Chart" << chartName << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_chartsModels.size(), m_chartsModels.size());
    ChartSeriesModel *seriesModel = new ChartSeriesModel(this);
    seriesModel->setParametersStorage(m_parametersStorage);
    m_chartsModels.append(seriesModel);
    m_chartNames.append(chartName);
    endInsertRows();
    
    qDebug() << "ChartsListModel: Added chart" << chartName;
}

void ChartsListModel::addChart(const QString &chartName, const QStringList &parameterLabels)
{
    if (hasChart(chartName)) {
        qDebug() << "ChartsListModel: Chart" << chartName << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_chartsModels.size(), m_chartsModels.size());
    ChartSeriesModel *seriesModel = new ChartSeriesModel(parameterLabels, this);
    seriesModel->setParametersStorage(m_parametersStorage);
    m_chartsModels.append(seriesModel);
    m_chartNames.append(chartName);
    endInsertRows();
    
    qDebug() << "ChartsListModel: Added chart" << chartName << "with" << parameterLabels.size() << "series";
}

void ChartsListModel::removeChart(const QString &chartName)
{
    int index = m_chartNames.indexOf(chartName);
    if (index >= 0) {
        removeChart(index);
    } else {
        qDebug() << "ChartsListModel: Chart" << chartName << "not found";
    }
}

void ChartsListModel::removeChart(int index)
{
    if (index < 0 || index >= m_chartsModels.size()) {
        qWarning() << "ChartsListModel: Invalid index" << index;
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    ChartSeriesModel *seriesModel = m_chartsModels.takeAt(index);
    QString removedName = m_chartNames.takeAt(index);
    if (seriesModel) {
        seriesModel->deleteLater();
    }
    endRemoveRows();
    
    qDebug() << "ChartsListModel: Removed chart" << removedName;
}

void ChartsListModel::clearCharts()
{
    if (m_chartsModels.isEmpty())
        return;
        
    beginResetModel();
    qDeleteAll(m_chartsModels);
    m_chartsModels.clear();
    m_chartNames.clear();
    endResetModel();
    
    qDebug() << "ChartsListModel: Cleared all charts";
}

void ChartsListModel::addSeriesToChart(const QString &chartName, const QString &parameterLabel)
{
    ChartSeriesModel *seriesModel = getSeriesModel(chartName);
    if (seriesModel) {
        seriesModel->addSeries(parameterLabel);
    } else {
        qWarning() << "ChartsListModel: Series model not found for chart" << chartName;
    }
}

void ChartsListModel::addSeriesToChart(const QString &chartName, const QStringList &parameterLabels)
{
    ChartSeriesModel *seriesModel = getSeriesModel(chartName);
    if (seriesModel) {
        seriesModel->addSeries(parameterLabels);
    } else {
        qWarning() << "ChartsListModel: Series model not found for chart" << chartName;
    }
}

void ChartsListModel::removeSeriesFromChart(const QString &chartName, const QString &parameterLabel)
{
    ChartSeriesModel *seriesModel = getSeriesModel(chartName);
    if (seriesModel) {
        seriesModel->removeSeries(parameterLabel);
    } else {
        qWarning() << "ChartsListModel: Series model not found for chart" << chartName;
    }
}

ChartSeriesModel* ChartsListModel::getSeriesModel(const QString &chartName) const
{
    int index = m_chartNames.indexOf(chartName);
    if (index >= 0 && index < m_chartsModels.size()) {
        return m_chartsModels.at(index);
    }
    return nullptr;
}

ChartSeriesModel* ChartsListModel::getSeriesModel(int index) const
{
    if (index >= 0 && index < m_chartsModels.size()) {
        return m_chartsModels.at(index);
    }
    return nullptr;
}

QStringList ChartsListModel::chartNames() const
{
    return m_chartNames;
}

bool ChartsListModel::hasChart(const QString &chartName) const
{
    return m_chartNames.contains(chartName);
}

bool ChartsListModel::hasChart(int index) const
{
    return index >= 0 && index < m_chartsModels.size();
}

void ChartsListModel::setParametersStorage(BoardParametersStorage *storage)
{
    m_parametersStorage = storage;
    if (m_parametersStorage) {
        connect(m_parametersStorage, &BoardParametersStorage::parameterAdded, 
                this, &ChartsListModel::onParameterAdded);
    }
    
    qDebug() << "ChartsListModel: Parameters storage set";
}

void ChartsListModel::onParameterAdded(const QString &label)
{
    // Передаем сигнал всем моделям серий
    for (ChartSeriesModel *seriesModel : m_chartsModels) {
        if (seriesModel) {
            seriesModel->handleParameterAdded(label);
        }
    }
    
    qDebug() << "ChartsListModel: Parameter added:" << label << "forwarded to all charts";
}
