#ifndef OUTPARAMETERMODEL_H
#define OUTPARAMETERMODEL_H

#include <QAbstractTableModel>
#include <QWidget>
#include <QTableView>
#include "Model/Parameters/OutParametersStorage.h"

class OutParametersModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum class OutParameterRole : int
    {
        LabelRole = Qt::UserRole + 1,
        ValueRole,
        DataControlRole,
    };
public:
    explicit OutParametersModel(QObject *parent = nullptr);
    ~OutParametersModel() override = default;

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;
    
    // Методы для работы с хранилищем
    void setStorage(OutParametersStorage *storage);
    OutParametersStorage* getStorage() const { return m_storage; }
    void refreshModel();
   
private:
    QVariantMap createParameterData(OutParameter* parameter) const;

private:
    void setupParameters();
    OutParametersStorage *m_storage;
};

#endif // OUTPARAMETERMODEL_H
