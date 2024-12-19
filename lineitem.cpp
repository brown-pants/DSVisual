#include "lineitem.h"
#include "nodeitem.h"
#include <QPainter>
#include <QVector2D>
#include <QtMath>
#include <QDebug>

LineItem::LineItem(type t, NodeItem *begin, NodeItem *end, QGraphicsItem *parent) : QGraphicsLineItem(parent)
{
    m_type = t;
    beginItem = begin;
    endItem = end;
    isArc = false;
    setZValue(-1);
}

void LineItem::updateLine()
{//根据起点与终点更新线
    QPointF p1 = mapFromItem(beginItem, 50, 50);
    QPointF p2 = mapFromItem(endItem, 50, 50);
    QVector2D vec(p2 - p1);
    vec.normalize();
    vec *= 50;
    p1 += vec.toPointF();
    p2 -= vec.toPointF();
    QLineF line(p1, p2);
    setLine(line);
}

void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(pen());
    if(m_type != NoArrow)
    {//画箭头
        float arrowAngle = 30.0f * 3.14f / 180.0f;
        int arrowLen = 12;
        QPointF p1;
        QPointF p2;
        QVector2D vec_1;
        QVector2D vec_2;

        if(isArc)
        {
            QPointF v1 = mapFromItem(beginItem, 50, 50);
            QPointF v2 = mapFromItem(endItem, 50, 50);
            qreal a = fabs(v2.x() - v1.x());
            qreal b = fabs(v2.y() - v1.y());
            QPointF o(v1.x(), v2.y());
            qreal x=0.0, y=0.0;

            bool flagX = v1.x() > v2.x() ? true : false;
            bool flagY = v1.y() > v2.y() ? true : false;

            //begin arrow
            for(x = v1.x(); flagX ? x >= v1.x() - 50 : x <= v1.x() + 50; x += flagX ? -0.1 : 0.1)
            {
                y = o.y() + (flagY ? 1 : -1) * b * sqrt(1.0 - pow(x - o.x(), 2) / (a * a));
                if(pow(x - v1.x(), 2) + pow(y - v1.y(), 2) >= 50 * 50) break;
            }
            p1 = QPointF(x, y);
            x += flagX ? -0.1 : 0.1;
            y = o.y() + (flagY ? 1 : -1) * b * sqrt(1.0 - pow(x - o.x(), 2) / (a * a));
            vec_2 = QVector2D(p1 - QPointF(x, y));

            //end arrow
            for(x = v2.x(); flagX ? x <= v2.x() + 50 : x >= v2.x() - 50; x += flagX ? 0.1 : -0.1)
            {
                y = o.y() + (flagY ? 1 : -1) * b * sqrt(1.0 - pow(x - o.x(), 2) / (a * a));
                if(pow(x - v2.x(), 2) + pow(y - v2.y(), 2) >= 50 * 50) break;
            }
            p2 = QPointF(x, y);
            x += flagX ? -0.1 : 0.1;
            y = o.y() + (flagY ? 1 : -1) * b * sqrt(1.0 - pow(x - o.x(), 2) / (a * a));
            vec_1 = QVector2D(p2 - QPointF(x, y));
        }
        else
        {
            p1 = line().p1();
            p2 = line().p2();
            vec_1 = QVector2D(p1 - p2);
            vec_2 = vec_1;
        }

        vec_1.normalize();
        vec_2.normalize();
        QVector2D vertical_1(-vec_1.y(), vec_1.x());
        QVector2D vertical_2(-vec_2.y(), vec_2.x());
        QVector2D dir_1 = sin(arrowAngle) * arrowLen * vertical_1 + cos(arrowAngle) * arrowLen * vec_1;
        QVector2D dir_2 = sin(arrowAngle) * arrowLen * -vertical_1 + cos(arrowAngle) * arrowLen * vec_1;
        QVector2D dir_3 = sin(arrowAngle) * arrowLen * vertical_2 + cos(arrowAngle) * arrowLen * vec_2;
        QVector2D dir_4 = sin(arrowAngle) * arrowLen * -vertical_2 + cos(arrowAngle) * arrowLen * vec_2;

        if(m_type == SingleArrow)
        {
            painter->drawLine(p2, p2 + dir_1.toPointF());
            painter->drawLine(p2, p2 + dir_2.toPointF());
        }
        else if(m_type == DoubleArrow)
        {
            painter->drawLine(p2, p2 + dir_1.toPointF());
            painter->drawLine(p2, p2 + dir_2.toPointF());
            painter->drawLine(p1, p1 - dir_3.toPointF());
            painter->drawLine(p1, p1 - dir_4.toPointF());
        }
    }

    if(isArc)
    {//画弧线
       QPointF p1 = mapFromItem(beginItem, 50, 50);
       QPointF p2 = mapFromItem(endItem, 50, 50);
       QRect rect = QRect(p2.x(), p1.y(), (p1.x() - p2.x()) * 2, (p2.y() - p1.y()) * 2);
       if(p1.x() > p2.x())
       {
           if(p1.y() > p2.y())
               painter->drawArc(rect, 180 * 16, 90 * 16);
           else painter->drawArc(rect, 90 * 16, 90 * 16);
       }
       else
       {
           if(p1.y() > p2.y())
               painter->drawArc(rect, 0 * 16, -90 * 16);
           else painter->drawArc(rect, 0 * 16, 90 * 16);
       }
    }
    else
    {//画直线
        QGraphicsLineItem::paint(painter, option, widget);
    }
}

QRectF LineItem::boundingRect() const
{
    QPointF p1;
    QPointF p2;
    if(isArc)
    {
        p1 = mapFromItem(beginItem, 50, 50);
        p2 = mapFromItem(endItem, 50, 50);
    }
    else
    {
        p1 = line().p1();
        p2 = line().p2();
    }
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(p1, QSizeF(p2.x() - p1.x(),
                                      p2.y() - p1.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
