#ifndef UPLINKPARAMETERMODEL_H
#define UPLINKPARAMETERMODEL_H

#include <QAbstractTableModel>
#include <QWidget>
#include <QTableView>
#include <QTimer>
#include "Model/Parameters/BasicUplinkParameter.h"

class UplinkParametersModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum class UplinkParameterRole : int
    {
        LabelRole = Qt::UserRole + 1,
        ValueRole,
        DataControlRole,
        ValueTypeRole,
    };

public:
    explicit UplinkParametersModel(QObject *parent = nullptr);
    ~UplinkParametersModel() override = default;

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int countParameters() const { return m_parameters.count(); }

signals:
    void parameterChanged(BasicUplinkParameter* parameter);

private slots:
    void onInitializationTimeout();

public:
    // Метод для программного изменения значения без отправки сигнала
    void setParameterValueSilently(int row, const QVariant &value);
    // Методы для работы с параметрами
    void setParameters(const QList<BasicUplinkParameter*> &parameters);
    const QList<BasicUplinkParameter*>& getParameters() const { return m_parameters; }
    void refreshModel();
   
private:
    QVariantMap createParameterData(BasicUplinkParameter* parameter) const;

private:
    void setupParameters();
    QList<BasicUplinkParameter*> m_parameters;
    bool m_isInitializing = false; // Флаг для отслеживания инициализации
    QTimer* m_initializationTimer; // Таймер для отслеживания завершения инициализации
};

#endif // UPLINKPARAMETERMODEL_H
