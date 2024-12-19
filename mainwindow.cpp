#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_thrdBinTree_triggered()
{//二叉树线索化
    if(centralWidget() != nullptr) centralWidget()->setParent(nullptr);
    this->setCentralWidget(&m_ThrdBinTreeWidget);
}

void MainWindow::on_action_binSortTree_triggered()
{//二叉搜索树
    if(centralWidget() != nullptr) centralWidget()->setParent(nullptr);
    this->setCentralWidget(&m_BinSortTreeWidget);
}
