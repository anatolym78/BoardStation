#ifndef BOARDPARAMETERSLISTMODEL_H
#define BOARDPARAMETERSLISTMODEL_H

#include <QAbstractListModel>

#include "Model/Parameters/BoardParameterHistoryStorage.h"

class BoardParametersListModel : public QAbstractListModel
{
	enum class ParameterRole: int
	{
        LabelRole = Qt::UserRole + 1,
		ValueRole,
	};
public:
    explicit BoardParametersListModel(BoardParameterHistoryStorage* storage, QObject *parent = nullptr);

    void setParametersStorage(BoardParameterHistoryStorage* boardParametersStorage);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    BoardParameterHistoryStorage* m_pParametersStorage;
};

#endif // BOARDPARAMETERSLISTMODEL_H
