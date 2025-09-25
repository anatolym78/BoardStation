#ifndef BOARDPARAMETERSLISTMODEL_H
#define BOARDPARAMETERSLISTMODEL_H

#include <QAbstractListModel>
#include <QColor>

#include "Model/Parameters/BoardParameterHistoryStorage.h"

class BoardParametersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
	enum class ParameterRole: int
	{
        LabelRole = Qt::UserRole + 1,
		ValueRole,
        UntiRole,
        TimeRole,
        ChartVisibilityRole,
        ColorRole,
	};
public:
    explicit BoardParametersListModel(BoardParameterHistoryStorage* storage, QObject *parent = nullptr);

    void setParametersStorage(BoardParameterHistoryStorage* boardParametersStorage);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int getCountParameters() const;

protected slots:
    void onNewParameterAdded(BoardParameterSingle* param);
	void onParameterAdded(const QString& label);
	void onParameterUpdated(const QString& label);
	void onParametersCleared();

private:
    BoardParameterHistoryStorage* m_pParametersStorage;
    QStringList m_parameterLabels; //
    QList<bool> m_chartVisibilities;
    QList<QColor> m_colors;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

#endif // BOARDPARAMETERSLISTMODEL_H
