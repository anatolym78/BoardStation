#ifndef BOARDPARAMETERSTREEMODEL_H
#define BOARDPARAMETERSTREEMODEL_H

#include <QAbstractItemModel>
#include <QColor>

#include "Model/Parameters/BoardParameterSingle.h"
#include "./Model/Parameters/Tree/ParameterTreeStorage.h"
#include "DataPlayer.h"

class BoardParameterTreeItem;

class BoardParametersTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int countParameters READ getCountParameters NOTIFY countParametersChanged)

public:
    enum ParameterRole
    {
        LabelRole = Qt::UserRole + 1,
        ValueRole,
        UnitRole,
        TimeRole,
        ChartVisibilityRole,
        ColorRole,
        FullPathRole
    };
    Q_ENUM(ParameterRole)

public:
    explicit BoardParametersTreeModel(QObject* parent = nullptr);
    ~BoardParametersTreeModel() override;

    void setStorage(ParameterTreeStorage* storage);
    void setPlayer(DataPlayer* dataPlayer);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int getCountParameters() const;

    Q_INVOKABLE void clearParameters();

signals:
    void countParametersChanged();

public slots:
    void onNewParameterAdded(BoardParameterSingle* parameter);
    void onParametersCleared();

private:
    void makeRandomColors();

    QString indexToLabel(int row) const;

private:
    ParameterTreeStorage* m_parametersTreeStorage = nullptr;
    BoardParameterTreeItem* m_rootItem;
    QList<bool> m_chartVisibilities;
    QList<QColor> m_colors;

    DataPlayer* m_dataPlayer = nullptr;
    QMetaObject::Connection m_playConnection;
    QMetaObject::Connection m_stopConnection;
};

#endif // BOARDPARAMETERSTREEMODEL_H
