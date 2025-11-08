#ifndef BOARDPARAMETERSTREEMODELPROXY_H
#define BOARDPARAMETERSTREEMODELPROXY_H

#include <QIdentityProxyModel>

class BoardParametersTreeModelProxy : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit BoardParametersTreeModelProxy(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

private:
    QVariant m_horizontalHeaders[2];
};

#endif // BOARDPARAMETERSTREEMODELPROXY_H
