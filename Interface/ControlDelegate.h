#ifndef CONTROLDELEGATE_H
#define CONTROLDELEGATE_H

#include <QStyledItemDelegate>

class ControlDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit ControlDelegate(QObject *parent = nullptr);

	// QStyledItemDelegate interface
	QWidget* createEditor(QWidget *parent, 
	                      const QStyleOptionViewItem &option, 
	                      const QModelIndex &index) const override;
	
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, 
	                  const QModelIndex &index) const override;
	
	void updateEditorGeometry(QWidget *editor, 
	                         const QStyleOptionViewItem &option, 
	                         const QModelIndex &index) const override;

private:
	bool isHistoryItem(const QModelIndex &index) const;
};

#endif // CONTROLDELEGATE_H

