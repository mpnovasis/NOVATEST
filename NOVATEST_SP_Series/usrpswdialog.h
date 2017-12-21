#ifndef USRPSWDIALOG_H
#define USRPSWDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class UsrPswDialog;
}

class UsrPswDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsrPswDialog(QWidget *parent = 0);
    ~UsrPswDialog();

private slots:
    void on_logIn_pushButton_clicked();

    void on_cancel_pushButton_clicked();



private:
    Ui::UsrPswDialog *ui;
};

#endif // USRPSWDIALOG_H
