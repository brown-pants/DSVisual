#ifndef TEXTITEM_H
#define TEXTITEM_H
#include <QGraphicsTextItem>

class TextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 2 };
    TextItem(QGraphicsItem *parent = nullptr);

protected:
    QRectF boundingRect() const override;
    inline int type() const override { return Type; }
};

#endif // TEXTITEM_H
