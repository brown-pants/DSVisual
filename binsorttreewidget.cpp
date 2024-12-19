#include "binsorttreewidget.h"
#include "ui_binsorttreewidget.h"

BinSortTreeWidget::BinSortTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BinSortTreeWidget)
{
    ui->setupUi(this);
    scene = new Scene(Scene::BinSortTreeScene, ui->graphicsView);
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

    connect(ui->button_order, &QPushButton::clicked, [=]{
        //升序遍历
        scene->showBinTreeTraversalAnimation(Inorder);
    });

    connect(ui->button_find, &QPushButton::clicked, [=]{
        //查找结点
        static TextInputDialog inputDialog;
        static bool isInputDialogInit = false;
        if(!isInputDialogInit)
        {
            isInputDialogInit = true;
            inputDialog.setWindowTitle("查找数据(整数)");
            static QIntValidator intValidator;
            inputDialog.setInputValidator(&intValidator);
            connect(&inputDialog, &TextInputDialog::inputted, [=](QString text){
                if(text.isEmpty()) return;
                int num = text.toInt();
                scene->showBinSortTreeFindNodeAnimation(num);
            });
        }
        inputDialog.setText("");
        inputDialog.exec();
    });
}

BinSortTreeWidget::~BinSortTreeWidget()
{
    delete ui;
}
