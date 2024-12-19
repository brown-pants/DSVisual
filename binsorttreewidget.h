#ifndef BINSORTTREEWIDGET_H
#define BINSORTTREEWIDGET_H

#include <QWidget>
#include "scene.h"

namespace Ui {
class BinSortTreeWidget;
}

class BinSortTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BinSortTreeWidget(QWidget *parent = nullptr);
    ~BinSortTreeWidget();

private:
    Ui::BinSortTreeWidget *ui;
    Scene *scene;
};

#endif // BINSORTTREEWIDGET_H
