#ifndef PARAMETERSLISTMODEL_H
#define PARAMETERSLISTMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include "../Model/Parameter.h"
#include "../Model/ParametersStorage.h"

class ParametersListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        LabelColumn = 0,
        ValueColumn = 1,
        UnitColumn = 2,
        TimestampColumn = 3
    };

    explicit ParametersListModel(ParametersStorage *storage, QObject *parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с данными
    void setParametersStorage(ParametersStorage *storage);
    void refreshModel();
    
    // Получение параметров
    Parameter getParameter(int index) const;
    Parameter getParameter(const QString &label) const;
    
    // Методы для работы с историей значений
    ParameterValue getLastValue(int index) const;
    ParameterValue getLastValue(const QString &label) const;
    QList<ParameterValue> getValueHistory(int index) const;
    QList<ParameterValue> getValueHistory(const QString &label) const;

private slots:
    void onParameterAdded(const QString &label);
    void onParameterUpdated(const QString &label);
    void onParametersCleared();

private:
    ParametersStorage *m_storage;
    QStringList m_parameterLabels; // Кэш меток для индексации строк
};

#endif // PARAMETERSLISTMODEL_H
