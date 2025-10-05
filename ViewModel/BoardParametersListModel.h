#ifndef BOARDPARAMETERSLISTMODEL_H
#define BOARDPARAMETERSLISTMODEL_H

#include <QAbstractListModel>
#include <QColor>

#include "Model/Parameters/BoardMessagesSqliteReader.h"

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
	explicit BoardParametersListModel(BoardMessagesSqliteReader* dbReader, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int getCountParameters() const;
    
    // Публичный метод для очистки параметров
    Q_INVOKABLE void clearParameters();

public slots :
    void onNewParameterAdded(BoardParameterSingle* param);
	void onParametersCleared();

private:
    BoardMessagesSqliteReader* m_pDbReader;
    QMap<QString, BoardParameterSingle*> m_values;
    QList<bool> m_chartVisibilities;
    QList<QColor> m_colors;

public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    void makeRandomColors();
};

#endif // BOARDPARAMETERSLISTMODEL_H
