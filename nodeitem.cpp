#include "nodeitem.h"
#include "scene.h"
#include <QPen>

NodeItem::NodeItem( NodeItemType t, QGraphicsItem *parent) : QGraphicsEllipseItem(parent)
{
    m_type = t;
    textItem = new TextItem(this);
    setRect(0, 0, 100, 100);
    QPen pen;
    pen.setWidth(2);
    setPen(pen);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    InitAnimation();

    //创建结点
    if(m_type == BinTreeNodeItem || m_type == BinSortTreeNodeItem)
    {
        if(m_type == BinTreeNodeItem)
            binTreeNode = new BinTreeNode<QString>;
        else binSortTreeNode = new BinTreeNode<int>;

        binTreeNode->left = nullptr;
        binTreeNode->right = nullptr;
        binTreeNode->p_item = this;
        //子树连线
        lines.push_back(nullptr);
        lines.push_back(nullptr);
        parent_lines.push_back(nullptr);
        parent_lines.push_back(nullptr);
        if(m_type == BinTreeNodeItem)
        {//线索线段
            lines.push_back(nullptr);
            lines.push_back(nullptr);
            parent_lines.push_back(nullptr);
            parent_lines.push_back(nullptr);
        }
    }
}

NodeItem::~NodeItem()
{
    if(m_type == BinTreeNodeItem || m_type == BinSortTreeNodeItem)
    {
        DeleteBinTreeNodeItem();
    }
    delete bounceAnim;
    //从场景中删除此结点
    scene()->removeItem(this);
}

void NodeItem::setBinTreeNodeData(const QString &text)
{
    binTreeNode->data = text;
    textItem->setPlainText(text);
}

void NodeItem::setBinSortTreeNodeData(int num)
{
    binSortTreeNode->data = num;
    textItem->setPlainText(QString::number(num));
}

NodeItem *NodeItem::getBinTreeParentNodeItem()
{
    auto iter_parentLine_left = parent_lines.begin();
    auto iter_parentLine_right = parent_lines.begin();
    iter_parentLine_right++;
    if(*iter_parentLine_left != nullptr)
    {
        return (*iter_parentLine_left)->getBeginItem();
    }
    else if(*iter_parentLine_right != nullptr)
    {
        return (*iter_parentLine_right)->getBeginItem();
    }
    else return nullptr;
}

void NodeItem::updateLines()
{
    for(auto it : lines)
    {
        if(it != nullptr) it->updateLine();
    }
    for(auto it : parent_lines)
    {
        if(it != nullptr) it->updateLine();
    }
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
    {//位置改变 更新连线
        updateLines();
    }
    return value;
}

void NodeItem::InitAnimation()
{
    //初始化弹动动画
    bounceAnim = new QTimeLine(700);
    bounceAnim->setFrameRange(0, 250);
    QEasingCurve curve = QEasingCurve::OutBounce;
    qreal baseRadius = 120;
    qreal difRadius = -20;
    connect(bounceAnim, &QTimeLine::frameChanged, [=](int frame){
        qreal curProgress = curve.valueForProgress(frame / 200.0);
        qreal curRadius = baseRadius + difRadius * curProgress;
        this->setScale(curRadius / 100);
        Scene *p_scene = dynamic_cast<Scene *>(scene());
        //更新场景
        p_scene->update();
    });
    connect(bounceAnim, &QTimeLine::finished, [=](){
        //当前动画结束
        Scene *p_scene = dynamic_cast<Scene *>(scene());
        QQueue<QTimeLine *> &animQueue = p_scene->getAnimationQueue();
        animQueue.pop_front();
        if(animQueue.size() > 0)
        {//播放下一个动画
            animQueue.front()->start();
        }
        else
        {//所有动画结束
            emit p_scene->AnimationFinished();
        }
    });
}

void NodeItem::DeleteBinTreeNodeItem()
{
    auto iter_parentLine_left = parent_lines.begin();
    auto iter_parentLine_right = parent_lines.begin();
    iter_parentLine_right++;
    auto iter_line_left = lines.begin();
    auto iter_line_right = lines.begin();
    iter_line_right++;
    NodeItem *parentItem = nullptr;

    //断开与父结点的连接
    if(*iter_parentLine_left != nullptr)
    {//该节点是父结点的左节点
        LineItem *&parentLine_left = (*iter_parentLine_left);
        parentItem = parentLine_left->getBeginItem();
        parentItem->getBinTreeNode()->left = nullptr;
        scene()->removeItem(parentLine_left);
        delete parentLine_left;
        parentLine_left = nullptr;
        *parentItem->getLines().begin() = nullptr;
    }
    else if(*iter_parentLine_right != nullptr)
    {//该节点是父结点的右节点
        LineItem *&parentLine_right = (*iter_parentLine_right);
        parentItem = parentLine_right->getBeginItem();
        parentItem->getBinTreeNode()->right = nullptr;
        scene()->removeItem(parentLine_right);
        delete parentLine_right;
        parentLine_right = nullptr;
        auto iter_lines_right = parentItem->getLines().begin();
        iter_lines_right++;
        *iter_lines_right = nullptr;
    }

    //断开与子节点的连接
    if(binTreeNode->left != nullptr)
    {//存在左子树
        binTreeNode->left = nullptr;
        LineItem *line_left = *iter_line_left;
        NodeItem *leftChildItem = line_left->getEndItem();
        *leftChildItem->parent_lines.begin() = nullptr;
        scene()->removeItem(line_left);
        delete line_left;
        line_left = nullptr;
    }
    if(binTreeNode->right != nullptr)
    {//存在右子树
        binTreeNode->right = nullptr;
        LineItem *line_right = *iter_line_right;
        NodeItem *rightChildItem = line_right->getEndItem();
        auto iter_rightChild_parentLine_right = rightChildItem->parent_lines.begin();
        iter_rightChild_parentLine_right++;
        *iter_rightChild_parentLine_right = nullptr;
        scene()->removeItem(line_right);
        delete line_right;
        line_right = nullptr;
    }

    //释放二叉树结点内存
    binTreeNode->p_item = nullptr;
    if(m_type == BinTreeNodeItem) delete binTreeNode;
    else delete binSortTreeNode;
    binTreeNode = nullptr;
}
