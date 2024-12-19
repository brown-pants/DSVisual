#include "thrdbintreewidget.h"
#include "ui_thrdbintreewidget.h"
#include <QDebug>

ThrdBinTreeWidget::ThrdBinTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThrdBinTreeWidget)
{
    ui->setupUi(this);
    scene = new Scene(Scene::BinTreeScene, ui->graphicsView);
    ui->graphicsView->setScene(scene);
    //抗锯齿
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    //区域选择
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    connect(scene, &Scene::BinTreeNodeCountChanged, [=](int nodeCount, int treeDepth){
        //更新结点数与深度
        QStringList stringlist = ui->label_nodeCount->text().split(':');
        QString nodeCountStr = *stringlist.begin() + ':' + QString::number(nodeCount);
        stringlist = ui->label_treeDepth->text().split(':');
        QString treeDepthStr = *stringlist.begin() + ':' + QString::number(treeDepth);
        ui->label_nodeCount->setText(nodeCountStr);
        ui->label_treeDepth->setText(treeDepthStr);
    });
    connect(ui->button_preorder, &QPushButton::clicked, [=](){
        //先序遍历
        scene->showBinTreeTraversalAnimation(Preorder);
    });
    connect(ui->button_inorder, &QPushButton::clicked, [=](){
        //中序遍历
        scene->showBinTreeTraversalAnimation(Inorder);
    });
    connect(ui->button_postorder, &QPushButton::clicked, [=](){
        //后序遍历
        scene->showBinTreeTraversalAnimation(Postorder);
    });
    connect(ui->button_preThrd, &QPushButton::clicked, [=](){
        //先序线索化
        scene->showBinTreeThreaded(Preorder);
    });
    connect(ui->button_inThrd, &QPushButton::clicked, [=](){
        //中线索化
        scene->showBinTreeThreaded(Inorder);
    });
    connect(ui->button_postThrd, &QPushButton::clicked, [=](){
        //后序线索化
        scene->showBinTreeThreaded(Postorder);
    });
    connect(ui->button_hideThrd, &QPushButton::clicked, [=](){
       //隐藏线索化
        scene->hideBinTreeThreaded();
        scene->thrdState = -1;
    });
}

ThrdBinTreeWidget::~ThrdBinTreeWidget()
{
    delete ui;
}
