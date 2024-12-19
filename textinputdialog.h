#ifndef TEXTINPUTDIALOG_H
#define TEXTINPUTDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QValidator>
#include "nodeitem.h"

namespace Ui {
class TextInputDialog;
}

class TextInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextInputDialog(QWidget *parent = nullptr);
    ~TextInputDialog();

    void setText(const QString &text);
    void setInputValidator(QValidator *validator);

protected:
    //bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::TextInputDialog *ui;
    NodeItem *focusItem;

signals:
    void inputted(QString text);
};

#endif // TEXTINPUTDIALOG_H
