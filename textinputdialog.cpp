#include "textinputdialog.h"
#include "ui_textinputdialog.h"
#include <QPushButton>
#include <QMouseEvent>

TextInputDialog::TextInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextInputDialog)
{
    ui->setupUi(this);
    setFixedSize(301, 90);
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    connect(okButton, &QPushButton::clicked, [=](){
        //点击Ok按钮 触发信号
        emit inputted(ui->lineEdit->text());
    });
}

TextInputDialog::~TextInputDialog()
{
    delete ui;
}

void TextInputDialog::setText(const QString &text)
{
    ui->lineEdit->setText(text);
}

void TextInputDialog::setInputValidator(QValidator *validator)
{
    ui->lineEdit->setValidator(validator);
}
