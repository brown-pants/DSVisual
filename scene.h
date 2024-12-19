#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <list>
#include <QMenu>
#include <QQueue>
#include "nodeitem.h"
#include "textinputdialog.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum type
    {
        BinTreeScene,
        BinSortTreeScene
    };
    enum mode
    {
        Normal,
        Linked
    };

    Scene(type t, QGraphicsView *view = nullptr);
    ~Scene();

    void showBinTreeTraversalAnimation(BinTreeOrder order);
    void showBinTreeThreaded(BinTreeOrder order);
    void hideBinTreeThreaded();

    void showBinSortTreeFindNodeAnimation(int num);

    inline QQueue<QTimeLine *> &getAnimationQueue() { return animQueue; }

public:
    int thrdState;

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    type m_type;
    mode m_mode;
    std::list<NodeItem *> nodeList;
    QGraphicsView *p_graphicsView;
    QMenu menu_pressSpace;
    QMenu menu_selectItem;
    QMenu menu_focusItem;
    QPointF mouseScenePos;
    LineItem *tempLine;
    BinTreeLinkDir binTreeLinkDir;

    bool isAnimationState;
    QQueue<QTimeLine *> animQueue;//动画队列

private:
    void BinTreeLinkNode(NodeItem *beginNodeItem, NodeItem *endNodeItem, BinTreeLinkDir dir);
    void InitBinTreeScene();

    void BinSortTreeInsertNode(int value);
    void BinSortTreeRemoveNode(NodeItem *item);
    void InitBinSortTreeScene();

signals:
    //二叉树结点数量改变信号
    void BinTreeNodeCountChanged(int nodeCount, int treeDepth);
    //场景所有动画结束信号
    void AnimationFinished();
};

#endif // SCENE_H
