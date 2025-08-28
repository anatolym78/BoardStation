#ifndef ENGINESMODEL_H
#define ENGINESMODEL_H

#include <QAbstractTableModel>
#include <QSlider>
#include <QWidget>

class EnginesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EnginesModel(QObject *parent = nullptr);
    ~EnginesModel() override = default;

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Методы для работы с данными
    void setEngineValue(int row, int value);
    int getEngineValue(int row) const;
    QString getEngineName(int row) const;
    QSlider* getSlider(int row) const;

private:
    struct EngineData {
        QString name;
        int value;
        QSlider *slider;
    };

    QList<EngineData> m_engines;
    
    void setupEngines();
};

#endif // ENGINESMODEL_H
