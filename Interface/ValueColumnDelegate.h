#ifndef VALUECOLUMNDELEGATE_H
#define VALUECOLUMNDELEGATE_H

#include <QStyledItemDelegate>

class ValueColumnDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ValueColumnDelegate(QObject *parent = nullptr);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // VALUECOLUMNDELEGATE_H

