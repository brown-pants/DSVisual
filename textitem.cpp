#include "textitem.h"
#include <QFont>
#include <QTextOption>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>

TextItem::TextItem(QGraphicsItem *parent) : QGraphicsTextItem(parent)
{
    //字体设置
    this->setDefaultTextColor(QColor(0, 0, 0));
    QFont font("Micosoft YaHei", 23, QFont::Bold);
    this->setFont(font);
    //文字居中
    this->moveBy(0, 25);
    this->document()->setPageSize(QSizeF(100, 0));
    this->document()->setDocumentMargin(0);
    this->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignVCenter));
}

QRectF TextItem::boundingRect() const
{
    return QRectF(0, 0, 100, 45);
}
