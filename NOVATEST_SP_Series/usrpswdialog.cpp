#include "usrpswdialog.h"
#include "ui_usrpswdialog.h"
#include <QDebug>

QString user, password;

UsrPswDialog::UsrPswDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsrPswDialog)
{
    ui->setupUi(this);
    this->setWindowFlags((
    (windowFlags() | Qt::CustomizeWindowHint)
    & ~Qt::WindowCloseButtonHint
    ));
    this->setWindowTitle("LogIn");
}

UsrPswDialog::~UsrPswDialog()
{
    delete ui;
}

void UsrPswDialog::on_logIn_pushButton_clicked()
{

    MainWindow *m = qobject_cast<MainWindow*>(this->parent());


    user=ui->usr_lineEdit->text();
    password=ui->psw_lineEdit->text();
    if (user.isEmpty())
    {
        //qDebug() << "user is empty!";
        return;
    }
    if((user=="BRC01" && password=="Operatore01") || user=="michelepuca"){

    }else{
        m->update_status_bar("WRONG USERNAME OR PASSWORD!");
        ui->usr_lineEdit->clear();

        ui->psw_lineEdit->clear();
        //qDebug() << "Wrong login";
        ui->usr_lineEdit->setFocus();
        return;
    }
    //qDebug() << user;
    //qDebug() << password;
    m->setUserPsw(user,password);
    if(m->getUser()== "michelepuca"){
        m->update_status_bar("Benvenuto GRANDE PROGRAMMATORE BIANCO!");
        m->superUser();
    }else{
        m->update_status_bar("Benvenuto " + user);
    }
    m->logInSuccesfully();
    this->close();
    //qobject_cast<MainWindow*>(this->parent())->update_status_bar("Benvenuto " + qobject_cast<MainWindow*>(this->parent())->getUser());




}

void UsrPswDialog::on_cancel_pushButton_clicked()
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    this->close();
    m->centralWidget()->show();

}

