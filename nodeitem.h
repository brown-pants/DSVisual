#ifndef NODEITEM_H
#define NODEITEM_H
#include <QGraphicsEllipseItem>
#include <QTimeLine>
#include <QObject>
#include "textitem.h"
#include "lineitem.h"
#include "tree.hpp"

class NodeItem :  public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 1 };
    enum NodeItemType
    {
        BinTreeNodeItem,
        BinSortTreeNodeItem
    };
    NodeItem(NodeItemType t, QGraphicsItem *parent = nullptr);
    ~NodeItem();

    void setBinTreeNodeData(const QString &text);
    void setBinSortTreeNodeData(int num);

    inline QString binTreeNodeData() const { return binTreeNode->data; }
    inline int binSortTreeNodeData() const { return binSortTreeNode->data; }

    inline BinTreeNode<QString> *getBinTreeNode() const { return binTreeNode; }
    inline BinTreeNode<int> *getBinSortTreeNode() const { return binSortTreeNode; }

    inline TextItem *getTextItem() { return textItem; }
    inline std::list<LineItem *> &getLines() { return lines; }
    inline std::list<LineItem *> &getParentLines() { return parent_lines; }

    inline QTimeLine *getBounceAnimation() const { return bounceAnim; }
    NodeItem *getBinTreeParentNodeItem();

    void updateLines();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    inline int type() const override { return Type; }

private:
    TextItem *textItem;
    union
    {
        BinTreeNode<QString> *binTreeNode;
        BinTreeNode<int> *binSortTreeNode;
    };
    std::list<LineItem *> lines;
    std::list<LineItem *> parent_lines;
    NodeItemType m_type;
    QTimeLine *bounceAnim;//上下弹动动画

private:
    void InitAnimation();
    void DeleteBinTreeNodeItem();
};

#endif // NODEITEM_H
