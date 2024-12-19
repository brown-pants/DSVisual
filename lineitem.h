#ifndef LINEITEM_H
#define LINEITEM_H
#include <QGraphicsLineItem>

class NodeItem;
class LineItem : public QGraphicsLineItem
{
public:
    enum type
    {
        NoArrow,     //  ————
        SingleArrow, //  ————>
        DoubleArrow  // <————>
    };
    LineItem(type t, NodeItem *begin, NodeItem *end, QGraphicsItem *parent = nullptr);
    inline void setBeginItem(NodeItem *begin) { beginItem = begin; }
    inline void setEndItem(NodeItem *end) { endItem = end; }
    inline NodeItem *getBeginItem() const { return beginItem; }
    inline NodeItem *getEndItem() const { return endItem; }
    inline void setArc(bool isArc) { this->isArc = isArc; }
    void updateLine();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

private:
    type m_type;
    NodeItem *beginItem;
    NodeItem *endItem;
    bool isArc;
};

#endif // LINEITEM_H
