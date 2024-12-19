#ifndef THRDBINTREEWIDGET_H
#define THRDBINTREEWIDGET_H

#include <QWidget>
#include "scene.h"

namespace Ui {
class ThrdBinTreeWidget;
}

class ThrdBinTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThrdBinTreeWidget(QWidget *parent = nullptr);
    ~ThrdBinTreeWidget();

private:
    Ui::ThrdBinTreeWidget *ui;
    Scene *scene;
};

#endif // THRDBINTREEWIDGET_H
