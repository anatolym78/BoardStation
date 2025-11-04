#ifndef BOARDPARAMETERTREEITEM_H
#define BOARDPARAMETERTREEITEM_H

#include <QVariant>
#include <QList>
#include <QMap>

class BoardParameterSingle;

class BoardParameterTreeItem
{
public:
	BoardParameterTreeItem();

protected:
	explicit BoardParameterTreeItem(BoardParameterTreeItem* parentItem, BoardParameterSingle* parameter);
	explicit BoardParameterTreeItem(BoardParameterTreeItem* parentItem, BoardParameterSingle* parameter, int valueIndex);

public:
	void addParameter(BoardParameterSingle* parameter);
	BoardParameterTreeItem* updateParameter(BoardParameterSingle* parameter);

	BoardParameterTreeItem* child(int row);
	int childCount() const;
	int row() const;

	bool isRoot() const { return m_parentItem == nullptr; }
	bool isLeaf() const { return m_childItems.count() == 0; }
	int level() const { return m_level; }
	BoardParameterTreeItem* parent() const { return m_parentItem; }
	BoardParameterSingle* parameter() const { return m_parameter; }

	QString label() const;
	QVariant value() const;

	bool hasParameter(BoardParameterSingle* parameter);
	BoardParameterTreeItem* findItem(BoardParameterSingle* parameter);


private:
	bool isLastLevel() const;

private:
	QString m_itemLabel;
	QVariant m_itemValue;
	QList<BoardParameterTreeItem*> m_childItems;
	BoardParameterTreeItem* m_parentItem = nullptr;
	BoardParameterSingle* m_parameter = nullptr;
	int m_level = -1;
	bool m_indexLabel = false;

public:
	~BoardParameterTreeItem();
};

#endif // BOARDPARAMETERTREEITEM_H
