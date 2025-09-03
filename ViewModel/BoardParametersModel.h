#ifndef BOARDPARAMETERSMODEL_H
#define BOARDPARAMETERSMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include "../Model/Parameters/BoardParameter.h"
#include "../Model/Parameters/BoardParametersStorage.h"

class BoardParametersModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        LabelColumn = 0,
        ValueColumn = 1,
        UnitColumn = 2,
        TimestampColumn = 3
    };

    explicit BoardParametersModel(BoardParametersStorage *storage, QObject *parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для работы с данными
    void setParametersStorage(BoardParametersStorage *storage);
    void refreshModel();
    
    // Получение параметров
    BoardParameter* getParameter(int index) const;
    BoardParameter* getParameter(const QString &label) const;
    
    // Методы для работы с историей значений
    BoardParameterValue* getLastValue(int index) const;
    BoardParameterValue* getLastValue(const QString &label) const;
    QList<BoardParameterValue*> getValueHistory(int index) const;
    QList<BoardParameterValue*> getValueHistory(const QString &label) const;

private slots:
    void onParameterAdded(const QString &label);
    void onParameterUpdated(const QString &label);
    void onParametersCleared();

private:
    BoardParametersStorage *m_storage;
    QStringList m_parameterLabels; // Кэш меток для индексации строк
};

#endif // BOARDPARAMETERSMODEL_H
