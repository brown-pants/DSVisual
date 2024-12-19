#include "scene.h"
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QMessageBox>
#include <QIntValidator>

static QColor BinTreeLeftLineColor(100, 100, 255);
static QColor BinTreeRightLineColor(255, 100, 100);

Scene::Scene(type t, QGraphicsView *view) : QGraphicsScene(view)
{
    m_type = t;
    m_mode = Normal;
    thrdState = -1;
    isAnimationState = false;
    p_graphicsView = view;
    p_graphicsView->setMouseTracking(true);
    tempLine = nullptr;
    setItemIndexMethod(QGraphicsScene::NoIndex);

    connect(this, &Scene::AnimationFinished, [=](){
        isAnimationState = false;
    });

    if(m_type == BinTreeScene)
    {//二叉树场景初始化
        InitBinTreeScene();
    }
    else if(m_type == BinSortTreeScene)
    {//二叉排序树场景初始化
        InitBinSortTreeScene();
    }
}

Scene::~Scene()
{//删除所有结点
    for(auto it = nodeList.begin(); it != nodeList.end(); )
    {
        delete *it;
        nodeList.erase(it++);
    }
}

void Scene::showBinTreeTraversalAnimation(BinTreeOrder order)
{
    if(*nodeList.begin() == nullptr) return;
    isAnimationState = true;
    for(auto it : animQueue)
    {//停止所有动画
        it->stop();
    }
    animQueue.clear();
    std::list<BinTreeNode<QString> *> orderNodeList;
    BinTree::Traversal(order, (*nodeList.begin())->getBinTreeNode(), orderNodeList);
    for(auto it : orderNodeList)
    {
        if(it == nullptr) continue;
        NodeItem *item = it->p_item;
        QTimeLine *anim = item->getBounceAnimation();
        animQueue.push_back(anim);
    }
    //播放第一个动画
    animQueue.front()->start();
}

void Scene::showBinSortTreeFindNodeAnimation(int num)
{
    if(*nodeList.begin() == nullptr)
    {
        QMessageBox::information(p_graphicsView, "提示", "请先插入数据");
        return;
    }
    isAnimationState = true;
    for(auto it : animQueue)
    {//停止所有动画
        it->stop();
    }
    animQueue.clear();
    static std::list<BinTreeNode<int> *> orderNodeList;
    BinSortTree::FindNode((*nodeList.begin())->getBinSortTreeNode(), num, orderNodeList);
    for(auto it : orderNodeList)
    {
        if(it == nullptr) continue;
        NodeItem *item = it->p_item;
        QTimeLine *anim = item->getBounceAnimation();
        animQueue.push_back(anim);
    }
    disconnect(this, &Scene::AnimationFinished, 0, 0);
    connect(this, &Scene::AnimationFinished, [&, num](){
        isAnimationState = false;
        if(orderNodeList.empty()) return;
        if((*orderNodeList.rbegin())->data != num)
        {
            QMessageBox::about(p_graphicsView, "查找结果", QString("未找到该数据,共遍历%1个结点").arg(orderNodeList.size()));
        }
        else
        {
            QMessageBox::about(p_graphicsView, "查找结果", QString("已找到该数据,共遍历%1个结点").arg(orderNodeList.size()));
        }
        orderNodeList.clear();
    });

    //播放第一个动画
    animQueue.front()->start();
}

void Scene::showBinTreeThreaded(BinTreeOrder order)
{//线索化
    thrdState = order;
    hideBinTreeThreaded();
    std::list<BinTreeNode<QString> *> orderNodeList;
    BinTree::Traversal(order, (*nodeList.begin())->getBinTreeNode(), orderNodeList);

    auto orderNodeList_frist = orderNodeList.begin();
    auto orderNodeList_last = orderNodeList.end();
    orderNodeList_last--;
    for(auto iter = orderNodeList.begin(); iter != orderNodeList.end(); iter++)
    {
        if((*iter)->left == nullptr && iter != orderNodeList_frist)
        {//左结点为空 且不为序列链表的头结点
            auto pre = iter;
            pre--;
            NodeItem *beginItem = (*iter)->p_item;
            NodeItem *endItem = (*pre)->p_item;
            LineItem *arc = new LineItem(LineItem::SingleArrow, beginItem, endItem);
            arc->setArc(true);
            QPen pen(BinTreeLeftLineColor);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(3);
            arc->setPen(pen);
            auto iter_thrdLine_pre = beginItem->getLines().begin();
            auto iter_child_parentThrdLine_pre = endItem->getParentLines().begin();
            for(int i = 0; i < 2; i++)
            {
                iter_thrdLine_pre++;
                iter_child_parentThrdLine_pre++;
            }
            *iter_thrdLine_pre = arc;
            *iter_child_parentThrdLine_pre = arc;
            addItem(arc);
        }
        if((*iter)->right == nullptr && iter != orderNodeList_last)
        {//右结点为空 且不为序列链表的尾结点
            auto next = iter;
            next++;
            NodeItem *beginItem = (*iter)->p_item;
            NodeItem *endItem = (*next)->p_item;
            LineItem *arc = new LineItem(LineItem::SingleArrow, beginItem, endItem);
            arc->setArc(true);
            QPen pen(BinTreeRightLineColor);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(3);
            arc->setPen(pen);
            auto iter_thrdLine_next = beginItem->getLines().begin();
            auto iter_child_parentThrdLine_next = endItem->getParentLines().begin();
            for(int i = 0; i < 3; i++)
            {
                iter_thrdLine_next++;
                iter_child_parentThrdLine_next++;
            }
            *iter_thrdLine_next = arc;
            *iter_child_parentThrdLine_next = arc;
            addItem(arc);
        }
    }
}

void Scene::hideBinTreeThreaded()
{//删除线索化连线
    for(auto it : nodeList)
    {
        auto iter_thrdLine_pre = it->getLines().begin();
        auto iter_thrdLine_next = it->getLines().begin();
        auto iter_parentThrdLine_pre = it->getParentLines().begin();
        auto iter_parentThrdLine_next = it->getParentLines().begin();
        for(int i = 0; i < 2; i++)
        {
            iter_thrdLine_pre++;
            iter_parentThrdLine_pre++;
        }
        for(int i = 0; i < 3; i++)
        {
            iter_thrdLine_next++;
            iter_parentThrdLine_next++;
        }
        if(*iter_thrdLine_pre != nullptr)
        {
            removeItem(*iter_thrdLine_pre);
            delete *iter_thrdLine_pre;
        }
        if(*iter_thrdLine_next != nullptr)
        {
            removeItem(*iter_thrdLine_next);
            delete *iter_thrdLine_next;
        }
        *iter_thrdLine_pre = nullptr;
        *iter_thrdLine_next = nullptr;
        *iter_parentThrdLine_pre = nullptr;
        *iter_parentThrdLine_next = nullptr;
    }
}

void Scene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(event->delta() > 0) p_graphicsView->scale(1.2, 1.2);
    else p_graphicsView->scale(1.0 / 1.2, 1.0 / 1.2);
    QGraphicsScene::wheelEvent(event);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mouseScenePos = event->scenePos();
    if(m_mode == Normal)
    {//普通模式下
        if(event->button() == Qt::RightButton && isAnimationState == false)
        {
            QPoint menuPos = event->screenPos() + QPoint(8, 8);
            if(selectedItems().size() >= 2)
            {//选中两个及以上元素 弹出menu_selectItem菜单
                menu_selectItem.exec(menuPos);
            }
            else if(focusItem() != nullptr)
            {//焦点元素存在 弹出menu_focusItem菜单
                menu_focusItem.exec(menuPos);
            }
            else
            {//无选择状态 弹出menu_pressSpace菜单
                menu_pressSpace.exec(menuPos);
            }
        }
    }
    else if(m_mode == Linked)
    {//连线模式下
        m_mode = Normal;
        QList<QGraphicsItem *> endItems = items(tempLine->line().p2());
        if(endItems.count() == 0)
        {//无效连线
            removeItem(tempLine);
            delete tempLine;
        }
        else
        {
            if(m_type == BinTreeScene)
            {//二叉树连线
                QGraphicsItem *endItem = endItems.first();
                NodeItem *beginNodeItem = tempLine->getBeginItem();
                //点中结点且不为起始结点 或点中结点上的文本框且不为起始结点上的文本框
                if((endItem->type() == NodeItem::Type && endItem != beginNodeItem) ||
                   (endItem->type() == TextItem::Type && endItem != beginNodeItem->getTextItem()))
                {
                    NodeItem *endNodeItem;
                    if(endItem->type() == TextItem::Type)
                    {//点中了文本框 转换为父结点
                        endNodeItem = dynamic_cast<NodeItem *>(dynamic_cast<TextItem *>(endItem)->parentItem());
                    }
                    else
                    {//点中了结点
                        endNodeItem = dynamic_cast<NodeItem *>(endItem);
                    }
                    if(endNodeItem->getBinTreeNode() != (*nodeList.begin())->getBinTreeNode())
                    {//连接非根结点
                        BinTreeLinkNode(tempLine->getBeginItem(), endNodeItem, binTreeLinkDir);
                    }
                    else
                    {
                        removeItem(tempLine);
                        delete tempLine;
                        QMessageBox::warning(p_graphicsView, "错误", "请勿连接根结点");
                    }
                }
                else
                {//无效连线
                    removeItem(tempLine);
                    delete tempLine;
                }

                //连线结束重新进行线索化
                if(thrdState != -1)
                {
                    showBinTreeThreaded(BinTreeOrder(thrdState));
                }
            }
            /*else if(m_type == TYPE)
            {

            }*/
        }
        tempLine = nullptr;
    }

    QGraphicsScene::mousePressEvent(event);
}

void Scene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {//空格键进入场景拖拽模式
        p_graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void Scene::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {//空格键退出场景拖拽模式
        p_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_mode == Linked)
    {//连线模式下 连线开始
        QPointF p1 = tempLine->mapFromItem(tempLine->getBeginItem(), 50, 50);
        QPointF p2 = event->scenePos();
        QVector2D vec(p2 - p1);
        vec.normalize();
        vec *= 50;
        p1 += vec.toPointF();
        QLineF line(p1, p2);
        tempLine->setLine(line);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Scene::BinTreeLinkNode(NodeItem *beginNodeItem, NodeItem *endNodeItem, BinTreeLinkDir dir)
{
        auto iter_childItem_parentLine_left = endNodeItem->getParentLines().begin();
        auto iter_childItem_parentLine_right = endNodeItem->getParentLines().begin();
        iter_childItem_parentLine_right++;
        LineItem *&childItem_parentLine_left = *iter_childItem_parentLine_left;
        LineItem *&childItem_parentLine_right = *iter_childItem_parentLine_right;
        if(*iter_childItem_parentLine_left != nullptr)
        {//该子树已有父结点 且为左子树 断开与父结点的连接
            NodeItem *parentItem = childItem_parentLine_left->getBeginItem();
            BinTreeNode<QString> *parentNode = parentItem->getBinTreeNode();
            parentNode->left = nullptr;
            *parentItem->getLines().begin() = nullptr;
            delete childItem_parentLine_left;
            childItem_parentLine_left = nullptr;
        }
        if(*iter_childItem_parentLine_right != nullptr)
        {//该子树已有父结点 且为右子树 断开与父结点的连接
            NodeItem *parentItem = childItem_parentLine_right->getBeginItem();
            BinTreeNode<QString> *parentNode = parentItem->getBinTreeNode();
            parentNode->right = nullptr;
            auto iter_child_parentLine_right = parentItem->getLines().begin();
            iter_child_parentLine_right++;
            *iter_child_parentLine_right = nullptr;
            delete childItem_parentLine_right;
            childItem_parentLine_right = nullptr;
        }

        if(dir == LinkLeft)
        {//连接左结点
            beginNodeItem->getBinTreeNode()->left = endNodeItem->getBinTreeNode();
            //第一根线代表左结点
            *beginNodeItem->getLines().begin() = tempLine;
            *endNodeItem->getParentLines().begin() = tempLine;
            tempLine->setBeginItem(beginNodeItem);
            tempLine->setEndItem(endNodeItem);
            tempLine->updateLine();
        }
        else if(dir == LinkRight)
        {//连接右结点
            beginNodeItem->getBinTreeNode()->right = endNodeItem->getBinTreeNode();
            //第二根线代表右结点
            auto begin_right = beginNodeItem->getLines().begin();
            auto end_right = endNodeItem->getLines().begin();
            auto parent_right = endNodeItem->getParentLines().begin();
            begin_right++;
            end_right++;
            parent_right++;
            *begin_right = tempLine;
            *parent_right = tempLine;
            tempLine->setBeginItem(beginNodeItem);
            tempLine->setEndItem(endNodeItem);
            tempLine->updateLine();
        }

        BinTreeNode<QString> *rootNode = (*nodeList.begin())->getBinTreeNode();
        //连接结束计算二叉树长度和深度
        emit BinTreeNodeCountChanged(BinTree::NodeCount(rootNode), BinTree::TreeDepth(rootNode));
}

void Scene::InitBinTreeScene()
{//二叉树场景初始化
    //创建根结点
    NodeItem *rootItem = new NodeItem(NodeItem::BinTreeNodeItem);
    rootItem->setBrush(Qt::yellow);
    rootItem->setBinTreeNodeData("root");
    rootItem->setZValue(1);
    nodeList.push_back(rootItem);
    addItem(rootItem);

    //菜单设置
    QAction *addNode = menu_pressSpace.addAction("添加结点");
    QAction *rmAllChild = menu_pressSpace.addAction("删除所有子结点");
    QAction *rmChildren = menu_selectItem.addAction("删除选中的结点");
    QAction *rmFocusChild = menu_focusItem.addAction("删除该结点");
    QAction *changeNodeText = menu_focusItem.addAction("修改数据");
    QAction *linkLeftChild = menu_focusItem.addAction("连接左子树");
    QAction *linkRightChild = menu_focusItem.addAction("连接右子树");

    connect(addNode, &QAction::triggered, [=](){
        //添加结点
        NodeItem *node = new NodeItem(NodeItem::BinTreeNodeItem);
        node->setBrush(Qt::green);
        static int id = 1;
        node->setBinTreeNodeData(QString::number(id++));
        node->setPos(mouseScenePos.x(), mouseScenePos.y());
        nodeList.push_back(node);
        addItem(node);
    });
    connect(rmAllChild, &QAction::triggered, [=](){
        //删除所有子结点
        auto iter = nodeList.begin();
        hideBinTreeThreaded();
        for(iter++; iter != nodeList.end(); )
        {
            delete *iter;
            nodeList.erase(iter++);
        }
        emit BinTreeNodeCountChanged(1, 1);
    });
    connect(rmChildren, &QAction::triggered, [=](){
        //删除选中的结点
        QList<QGraphicsItem *> items = selectedItems();
        //先断开线索连接 再删除结点
        hideBinTreeThreaded();
        for(auto iter = items.begin(); iter != items.end(); )
        {
            if(*iter == *nodeList.begin())
            {
                QMessageBox::warning(p_graphicsView, "错误", "请勿删除根结点");
                iter++;
            }
            else
            {
                delete *iter;
                nodeList.erase(std::find(nodeList.begin(), nodeList.end(), *iter++));
            }
        }
        BinTreeNode<QString> *rootNode = (*nodeList.begin())->getBinTreeNode();
        //删除结束计算二叉树长度和深度
        emit BinTreeNodeCountChanged(BinTree::NodeCount(rootNode), BinTree::TreeDepth(rootNode));
        //删除结束重新进行线索化
        if(thrdState != -1)
        {
            showBinTreeThreaded(BinTreeOrder(thrdState));
        }
    });
    connect(rmFocusChild, &QAction::triggered, [=](){
        //删除焦点结点
        QGraphicsItem *item = focusItem();
        auto item_iterator = std::find(nodeList.begin(), nodeList.end(), item);
        if(item_iterator == nodeList.begin())
        {
            QMessageBox::warning(p_graphicsView, "错误", "请勿删除根结点");
        }
        else
        {
            //先断开线索连接 再删除结点
            hideBinTreeThreaded();
            delete item;
            nodeList.erase(item_iterator);
            BinTreeNode<QString> *rootNode = (*nodeList.begin())->getBinTreeNode();
            //删除结束计算二叉树长度和深度
            emit BinTreeNodeCountChanged(BinTree::NodeCount(rootNode), BinTree::TreeDepth(rootNode));
            //删除结束重新进行线索化
            if(thrdState != -1)
            {
                showBinTreeThreaded(BinTreeOrder(thrdState));
            }
        }
    });
    connect(changeNodeText, &QAction::triggered, [=](){
        //修改结点数据
        static TextInputDialog inputDialog;
        static bool isDialogInit = false;
        if(!isDialogInit)
        {
            isDialogInit = true;
            inputDialog.setWindowTitle("修改数据");
            connect(&inputDialog, &TextInputDialog::inputted, [=](QString text){
                NodeItem *item = dynamic_cast<NodeItem *>(this->focusItem());
                item->setBinTreeNodeData(text);
            });
        }
        NodeItem *item = dynamic_cast<NodeItem *>(focusItem());
        inputDialog.setText(item->binTreeNodeData());
        inputDialog.exec();
    });
    connect(linkLeftChild, &QAction::triggered, [=](){
        //连接左子树
        NodeItem *beginItem = dynamic_cast<NodeItem *>(focusItem());
        if(beginItem->getBinTreeNode()->left != nullptr)
        {//已有左子树 断开左子树连接
            beginItem->getBinTreeNode()->left = nullptr;
            (*(*beginItem->getLines().begin())->getEndItem()->getParentLines().begin()) = nullptr;
            delete (*beginItem->getLines().begin());
            (*beginItem->getLines().begin()) = nullptr;
        }
        tempLine = new LineItem(LineItem::NoArrow, beginItem, nullptr);
        QPen pen(BinTreeLeftLineColor);
        pen.setWidth(3);
        tempLine->setPen(pen);
        addItem(tempLine);
        tempLine->setLine(QLineF(beginItem->mapFromItem(beginItem, 50, 50), beginItem->mapFromItem(beginItem, 50, 50)));
        binTreeLinkDir = LinkLeft;
        m_mode = Linked;
    });
    connect(linkRightChild, &QAction::triggered, [=](){
        //连接右子树
        NodeItem *beginItem = dynamic_cast<NodeItem *>(focusItem());
        if(beginItem->getBinTreeNode()->right != nullptr)
        {//已有右子树 断开右子树连接
            beginItem->getBinTreeNode()->right = nullptr;
            auto right = beginItem->getLines().begin();
            right++;
            auto it_parentLine = (*right)->getEndItem()->getParentLines().begin();
            it_parentLine++;
            (*it_parentLine) = nullptr;
            delete *right;
            *right = nullptr;
        }
        tempLine = new LineItem(LineItem::NoArrow, beginItem, nullptr);
        QPen pen(BinTreeRightLineColor);
        pen.setWidth(3);
        tempLine->setPen(pen);
        addItem(tempLine);
        tempLine->setLine(QLineF(beginItem->mapFromItem(beginItem, 50, 50), beginItem->mapFromItem(beginItem, 50, 50)));
        binTreeLinkDir = LinkRight;
        m_mode = Linked;
    });
}

void Scene::BinSortTreeInsertNode(int value)
{//二叉排序树插入结点
    if(nodeList.empty())
    {//无结点插入根结点
        NodeItem *rootItem = new NodeItem(NodeItem::BinSortTreeNodeItem);
        rootItem->setBrush(Qt::yellow);
        rootItem->setBinSortTreeNodeData(value);
        rootItem->setZValue(1);
        nodeList.push_back(rootItem);
        addItem(rootItem);
    }
    else
    {
        std::list<BinTreeNode<int> *>list;
        BinSortTree::FindNode((*nodeList.begin())->getBinSortTreeNode(), value, list);
        if((*list.rbegin())->data == value)
        {
            QMessageBox::warning(p_graphicsView, "错误", "请勿插入相同数据");
            return;
        }
        //创建结点并连线
        NodeItem *parentItem = nullptr;
        NodeItem *childItem = new NodeItem(NodeItem::BinSortTreeNodeItem);
        childItem->setBrush(Qt::green);
        childItem->setBinSortTreeNodeData(value);
        nodeList.push_back(childItem);
        addItem(childItem);
        parentItem = BinSortTree::InsertNode((*nodeList.begin())->getBinSortTreeNode(), value, binTreeLinkDir)->p_item;
        tempLine = new LineItem(LineItem::NoArrow, parentItem, childItem);
        QPen pen;
        pen.setWidth(3);
        if(binTreeLinkDir == LinkLeft)
        {
            pen.setColor(BinTreeLeftLineColor);
        }
        else if(binTreeLinkDir == LinkRight)
        {
            pen.setColor(BinTreeRightLineColor);
        }
        tempLine->setPen(pen);
        addItem(tempLine);
        BinTreeLinkNode(parentItem, childItem, binTreeLinkDir);
        //移动结点
        QPointF parentItemPos = parentItem->pos();
        QPointF childItemPos;
        //重叠结点列表
        QList<QGraphicsItem *> overlapItemList;
        if(binTreeLinkDir == LinkLeft)
        {
            childItemPos = parentItemPos + QPointF(-200, 200);
            overlapItemList = items(childItemPos + QPointF(50,50));
            childItem->setPos(childItemPos);
        }
        else if(binTreeLinkDir == LinkRight)
        {
            childItemPos = parentItemPos + QPointF(200, 200);
            overlapItemList = items(childItemPos + QPointF(50,50));
            childItem->setPos(childItemPos);
        }
        //如果结点有重叠 调整位置
        if(!overlapItemList.empty())
        {
            //获取重叠的结点
            NodeItem *overlapNodeItem = nullptr;
            QGraphicsItem *overlapItem = *overlapItemList.begin();
            if(overlapItem->type() == TextItem::Type)
            {
                overlapNodeItem = dynamic_cast<NodeItem *>(dynamic_cast<TextItem *>(overlapItem)->parentItem());
            }
            else if(overlapItem->type() == NodeItem::Type)
            {
                overlapNodeItem = dynamic_cast<NodeItem *>(overlapItem);
            }
            //获取这两个重叠结点所在树的根结点
            NodeItem *parentItem_1 = childItem;
            NodeItem *parentItem_2 = overlapNodeItem;
            do{
                parentItem_1 = parentItem_1->getBinTreeParentNodeItem();
                parentItem_2 = parentItem_2->getBinTreeParentNodeItem();
            }while(parentItem_1 != parentItem_2);
            //移动左子树和右子树结点位置
            std::list<BinTreeNode<int> *> itemList;
            while(parentItem_1 != nullptr)
            {
                itemList.clear();
                BinTree::Traversal(Preorder, parentItem_1->getBinSortTreeNode()->left, itemList);
                for(auto it : itemList)
                {
                    it->p_item->moveBy(-200, 0);
                    it->p_item->updateLines();
                }
                itemList.clear();
                BinTree::Traversal(Preorder, parentItem_1->getBinSortTreeNode()->right, itemList);
                for(auto it : itemList)
                {
                    it->p_item->moveBy(200, 0);
                    it->p_item->updateLines();
                }
                parentItem_1 = parentItem_1->getBinTreeParentNodeItem();
            }
        }
        tempLine->updateLine();
    }

    BinTreeNode<int> *root = (*nodeList.begin())->getBinSortTreeNode();
    emit BinTreeNodeCountChanged(BinTree::NodeCount(root), BinTree::TreeDepth(root));
}

void Scene::BinSortTreeRemoveNode(NodeItem *item)
{//二叉排序树删除结点
    auto iter_item = std::find(nodeList.begin(), nodeList.end(), item);
    if(iter_item == nodeList.end()) return;

    BinTreeNode<int> *node = item->getBinSortTreeNode();
    if(node->left == nullptr && node->right == nullptr)
    {//无左右孩子
        nodeList.erase(iter_item);
        delete item;
    }
    else if(node->left != nullptr && node->right != nullptr)
    {//有左右孩子
        //在其左子树上找中序最后一个结点
        std::list<BinTreeNode<int> *> inorderList;
        BinTree::Traversal(Inorder, node->left, inorderList);
        BinTreeNode<int> * maxNode = *inorderList.rbegin();
        NodeItem * maxNodeItem = maxNode->p_item;
        item->setBinSortTreeNodeData(maxNode->data);
        BinSortTreeRemoveNode(maxNodeItem);
    }
    else
    {//只有左孩子或右孩子
        auto iter_parentLine_left = item->getParentLines().begin();
        auto iter_parentLine_right = item->getParentLines().begin();
        iter_parentLine_right++;
        if(*iter_parentLine_left == nullptr && *iter_parentLine_right == nullptr)
        {//无父结点 (该节点为根结点)
            nodeList.erase(iter_item);
            delete item;
            auto iter_root = nodeList.begin();
            (*iter_root)->setBrush(Qt::yellow);
        }
        else
        {//有父结点
            NodeItem *parentItem = nullptr;
            NodeItem *childItem = nullptr;
            QPen pen;
            pen.setWidth(3);
            if(*iter_parentLine_left != nullptr)
            {//该结点为父结点的左孩子
                parentItem = (*iter_parentLine_left)->getBeginItem();
                pen.setColor(BinTreeLeftLineColor);
                binTreeLinkDir = LinkLeft;
            }
            else if(*iter_parentLine_right != nullptr)
            {//该结点为父结点的右孩子
                parentItem = (*iter_parentLine_right)->getBeginItem();
                pen.setColor(BinTreeRightLineColor);
                binTreeLinkDir = LinkRight;
            }

            if(node->left != nullptr)
            {
                childItem = node->left->p_item;
            }
            else if(node->right != nullptr)
            {
                childItem = node->right->p_item;
            }
            //位置偏移量
            QPointF offset = item->pos() - childItem->pos();
            delete item;
            nodeList.erase(iter_item);
            tempLine = new LineItem(LineItem::NoArrow, parentItem, childItem);
            tempLine->setPen(pen);
            BinTreeLinkNode(parentItem, childItem, binTreeLinkDir);
            addItem(tempLine);
            tempLine->updateLine();
            //移动结点
            std::list<BinTreeNode<int> *> list;
            BinTree::Traversal(Inorder, childItem->getBinSortTreeNode(), list);
            for(auto it : list)
            {
                it->p_item->moveBy(offset.x(), offset.y());
                //更新连线
                it->p_item->updateLines();
            }
        }
    }
}

void Scene::InitBinSortTreeScene()
{//二叉排序树场景初始化
    QAction *addNode = menu_pressSpace.addAction("添加结点");
    QAction *rmAllChild = menu_pressSpace.addAction("删除所有结点");
    QAction *rmFocusChild = menu_focusItem.addAction("删除该结点");

    connect(addNode, &QAction::triggered, [=](){
        //添加结点
        static TextInputDialog inputDialog;
        static bool isInputDialogInit = false;
        if(!isInputDialogInit)
        {
            isInputDialogInit = true;
            inputDialog.setWindowTitle("插入数据(整数)");
            static QIntValidator intValidator;
            inputDialog.setInputValidator(&intValidator);
            connect(&inputDialog, &TextInputDialog::inputted, [=](QString text){
                if(text.isEmpty()) return;
                int num = text.toInt();
                BinSortTreeInsertNode(num);
            });
        }
        inputDialog.setText("");
        inputDialog.exec();
    });

    connect(rmAllChild, &QAction::triggered, [=](){
        //删除所有结点
        for(auto iter = nodeList.begin(); iter != nodeList.end(); )
        {
            delete *iter;
            nodeList.erase(iter++);
        }
        emit BinTreeNodeCountChanged(0, 0);
    });

    connect(rmFocusChild, &QAction::triggered, [=](){
        //删除焦点结点
        QGraphicsItem *item = focusItem();
        NodeItem *nodeItem = dynamic_cast<NodeItem *>(item);
        BinSortTreeRemoveNode(nodeItem);
        if(nodeList.empty())
        {
            emit BinTreeNodeCountChanged(0, 0);
        }
        else
        {
            BinTreeNode<int> *rootNode = (*nodeList.begin())->getBinSortTreeNode();
            //删除结束计算二叉树长度和深度
            emit BinTreeNodeCountChanged(BinTree::NodeCount(rootNode), BinTree::TreeDepth(rootNode));
        }
    });
}
