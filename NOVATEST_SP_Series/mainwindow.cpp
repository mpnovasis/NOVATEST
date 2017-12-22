#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>

bool superuser = false;
int connectedNOVAsom=0;
QSerialPort *serialport;
QSerialPort serial[MAXPORT];

QString userConfigDir = "C:/NOVATEST_SP/CONFIG/user/";
QString ConfigDir = "C:/NOVATEST_SP/CONFIG/CONF";
QString lan = "C:/NOVATEST_SP/CONFIG/xampp/htdocs/";
QString LogDir = "C:/NOVATEST_SP/CONFIG/log/";
QString MACdir = "C:/NOVATEST_SP/CONFIG/log/MAC/";
QString Fail ="C:/NOVATEST_SP/CONFIG/log/FAIL/";
QString directory = "C:/NOVATEST_SP/TESTED/";
QString labeltemplate= "C:/NOVATEST_SP/CONFIG/CONF/zebra_template.txt";



void clearstdstr( char *stringa){
    for(int i=0;i<strlen(stringa);i++){
        stringa[i]='\0';
    }
    return;
}

QString getvalue(QString strKey, QSettings *settings , QString entry)
{
    return settings->value( strKey + entry, "r").toString();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->save_configuration_pushButton->hide();
    ui->load_configuration_pushButton->hide();

    update_status_bar("Please LogIn...");
    setWindowTitle("NOVAtest Ver. 1.1.0b");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getParameter(){
    QString param;

    param.append(ui->PN_lineEdit->text());
    param.append("_");
    param.append(ui->Lotto_lineEdit->text());
    param.append("_");
    param.append(ui->seriale_lineEdit->text());

    return param;
}

int MainWindow::update_status_bar(QString StatusBarContent){

    statusBar()->showMessage(StatusBarContent);
    return 0;
}

QString MainWindow::getUser()
{
    return user;
}

QString MainWindow::getPsw()
{
    return psw;
}

void MainWindow::setUserPsw(QString utente, QString password)
{
    user=utente;
    psw=password;
    return;
}

void MainWindow::superUser(){
    ui->load_configuration_pushButton->show();
    ui->load_configuration_pushButton->setEnabled(true);
    ui->save_configuration_pushButton->show();
    ui->save_configuration_pushButton->setEnabled(true);
    superuser=true;
    logInSuccesfully();
    enablepanel();
    return;
}

void MainWindow::logInSuccesfully(){
    QList<QNetworkInterface> entries = QNetworkInterface::allInterfaces();
           for (int i=0;i<entries.count();i++){
               QNetworkInterface iface= entries.at(i);
               if(iface.flags().testFlag(QNetworkInterface::IsUp )){
                   for( int j =0 ; j< iface.addressEntries().count();j++){
                       if(iface.addressEntries().at(j).ip().protocol()==QAbstractSocket::IPv4Protocol){                           
                           if(strstr(iface.name().toUtf8(),"ethernet")){
                               ip = iface.addressEntries().at(j).ip().toString();
                               qDebug() << iface.addressEntries().at(j).ip().toString();
                               qDebug() << iface.name();
                               //ip = iface.addressEntries().at(j).ip().toString();
                               qDebug() << "vaccagialla";
                               qDebug() << ip;
                               qDebug() << iface.addressEntries().at(j).ip().protocol();
                           }
                           //if(iface.){

                           //}
                       }


               }
            }
           }
           if(ip==NULL) qDebug() << "NOINTERNETCONNECTION";

    popolate_line_combobox();
    if (loadpreviousconfiguration()==1){ //non ottimizzato secondo me!!
        on_line_comboBox_activated(ui->line_comboBox->currentText());
        loadpreviousconfiguration();
        on_model_comboBox_activated(ui->model_comboBox->currentText());
    }
    ui->logIn_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(true);
    ui->line_comboBox->setEnabled(true);
}

void MainWindow::popolate_line_combobox(){

    QString filename;
    filename=(ConfigDir+"/line.ini");
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire linecombobox!";

    }else{
        qDebug() << "File aperto correttamente";
    }
    ui->line_comboBox->clear();
    QString line;
    QTextStream in(&file);
    while (!in.atEnd()){
        line = in.readLine();
        if(line==""){ //la riga vuota è la fine della configuraione delle schede..
            break;
        }
        ui->line_comboBox->addItem(line);
    }
    file.close();
    return;
}

void MainWindow::enablepanel(){
    ui->audio_checkBox->setEnabled(true);
    ui->bt_checkBox->setEnabled(true);
    ui->console_checkBox->setEnabled(true);
    ui->cpu_checkBox->setEnabled(true);
    ui->eeprom_checkBox->setEnabled(true);
    ui->eMMC_checkBox->setEnabled(true);
    ui->eth_checkBox->setEnabled(true);
    ui->hdmi_checkBox->setEnabled(true);
    ui->pcie_checkBox->setEnabled(true);
    ui->ram_checkBox->setEnabled(true);
    ui->rtc_checkBox->setEnabled(true);
    ui->sata_checkBox->setEnabled(true);
    ui->sdcard_checkBox->setEnabled(true);
    ui->sensors_checkBox->setEnabled(true);
    ui->touch_checkBox->setEnabled(true);
    ui->usb_checkBox->setEnabled(true);
    ui->wifi_checkBox->setEnabled(true);

    return;
}

void MainWindow::disablepanel(){
    ui->audio_checkBox->setEnabled(false);
    ui->bt_checkBox->setEnabled(false);
    ui->console_checkBox->setEnabled(false);
    ui->cpu_checkBox->setEnabled(false);
    ui->eeprom_checkBox->setEnabled(false);
    ui->eMMC_checkBox->setEnabled(false);
    ui->eth_checkBox->setEnabled(false);
    ui->hdmi_checkBox->setEnabled(false);
    ui->pcie_checkBox->setEnabled(false);
    ui->ram_checkBox->setEnabled(false);
    ui->rtc_checkBox->setEnabled(false);
    ui->sata_checkBox->setEnabled(false);
    ui->sdcard_checkBox->setEnabled(false);
    ui->sensors_checkBox->setEnabled(false);
    ui->touch_checkBox->setEnabled(false);
    ui->usb_checkBox->setEnabled(false);
    ui->wifi_checkBox->setEnabled(false);


    return;
}

void MainWindow::on_logIn_pushButton_clicked()
{
    UsrPswDialog *p = new UsrPswDialog(this);
    p->show();
}

void MainWindow::on_logout_pushButton_clicked()
{
    if(connectedNOVAsom>0){
        on_disconnect_novasom_pushButton_clicked();
    }
    if(superuser==true){
        superuser=false;
        ui->save_configuration_pushButton->hide();
        ui->load_configuration_pushButton->hide();
        disablepanel();
    }
    if(user.isNull()==false && psw.isNull()==false){
        user.clear();
        psw.clear();
    }
    ui->logIn_pushButton->setEnabled(true);
    ui->logout_pushButton->setEnabled(false);
    ui->line_comboBox->setEnabled(false);
    ui->model_comboBox->setEnabled(false);
    ui->connect_novasom_pushButton->setEnabled(false);
}

void MainWindow::on_disconnect_novasom_pushButton_clicked()
{
    int j=0;
    for(j=0;j<connectedNOVAsom;j++) {
       if(serial[j].isOpen()==true){
           serial[j].close();
           if (serial[j].isOpen()==false){
           }else{
           }
       }else{
       }
    }
    if (j==connectedNOVAsom){
        connectedNOVAsom=0;
        ui->line_comboBox->setEnabled(true);
        ui->model_comboBox->setEnabled(true);
        ui->connect_novasom_pushButton->setEnabled(true);
        ui->disconnect_novasom_pushButton->setEnabled(false);
        ui->runtest_pushButton->setEnabled(false);
        ui->logout_pushButton->setEnabled(true);
    }else{
    }
    savelastsetting();
    if(superuser){
        enablepanel();
    }
    return;
}

void MainWindow:: savelastsetting(){

    QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         return;
     }
     QFile file(filename);
     if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        return;
         }
     QTextStream out(&file);
     out << ui->line_comboBox->currentText().append("\n");
     out << ui->model_comboBox->currentText();
     file.close();
    return;
}

void MainWindow::on_line_comboBox_activated(const QString &arg1)
{
    QString filename=(ConfigDir+"/"+arg1+".ini");
     QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
    }
    ui->model_comboBox->clear();
    ui->connect_novasom_pushButton->setEnabled(false);
    QString line;
    QTextStream in(&file);
    while (!in.atEnd()){
        line = in.readLine();
        if(line==""){ //la riga vuota è la fine della configuraione delle schede..
            break;
        }
        ui->model_comboBox->addItem(line);
    }
     ui->model_comboBox->setEnabled(true);
     update_status_bar("Line selected : "+ arg1);
     file.close();
    return;
}

void MainWindow::on_model_comboBox_activated(const QString &arg1)
{
    QString filename;
    filename=(ConfigDir+"/"+arg1+".ini");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire "+ filename;
        update_status_bar("[ERROR] No configuration file...Please contact us");
    }else{
        qDebug() << "File aperto correttamente";
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;
        settings = new QSettings( filename, QSettings::IniFormat );
        if ( getvalue(strKeyFunc, settings , "audio_checkBox") == "true")
            {
                qDebug() << "true";
                ui->audio_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->audio_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "bt_checkBox") == "true")
            {
                qDebug() << "true";
                ui->bt_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->bt_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "console_checkBox") == "true")
            {
                qDebug() << "true";
                ui->console_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->console_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "cpu_checkBox") == "true")
            {
                qDebug() << "true";
                ui->cpu_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->cpu_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eeprom_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eeprom_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eeprom_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eMMC_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eMMC_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eMMC_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eth_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eth_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eth_checkBox->setChecked(false);
            }

        if ( getvalue(strKeyFunc, settings , "hdmi_checkBox") == "true")
            {
                qDebug() << "true";
                ui->hdmi_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->hdmi_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "pcie_checkBox") == "true")
            {
                qDebug() << "true";
                ui->pcie_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->pcie_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "ram_checkBox") == "true")
            {
                qDebug() << "true";
                ui->ram_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->ram_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "rtc_checkBox") == "true")
            {
                qDebug() << "true";
                ui->rtc_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->rtc_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sata_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sata_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sata_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sdcard_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sdcard_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sdcard_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sensors_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sensors_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sensors_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "touch_checkBox") == "true")
            {
                qDebug() << "true";
                ui->touch_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->touch_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "usb_checkBox") == "true")
            {
                qDebug() << "true";
                ui->usb_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->usb_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "wifi_checkBox") == "true")
            {
                qDebug() << "true";
                ui->wifi_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->wifi_checkBox->setChecked(false);
            }
            file.close();
    }
    ui->connect_novasom_pushButton->setEnabled(true);
    update_status_bar("Model selected: : "+ arg1);
    file.close();
}

int MainWindow:: loadpreviousconfiguration(){
     QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         qDebug() << "fileName is empty";
         return -1;
     }
     QFile file(filename);
     if(!file.exists()) return -1;
     qDebug ()<< filename << "sto cercando di aprire";
     if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Non sono riuscito ad aprire il file!";
        return -1;
         }
     QString line;
     QTextStream in(&file);
     line = in.readLine();
     //qDebug () << "ciaooooooooooooooooooooooooo " << line;
     ui->line_comboBox->setCurrentText(line);
     line = in.readLine();
     //qDebug () << "ciaooooooooooooooooooooooooo " << line;
     ui->model_comboBox->setCurrentText(line);

     file.close();
     return 1;

}

int initPort(int num,QSerialPortInfo info){
    serial[num].setPort(info);
    serialport=&serial[num];
    if (serial[num].open(QIODevice::ReadWrite))
    {
        serial[num].setBaudRate(QSerialPort::Baud115200);
        serial[num].setParity(QSerialPort::NoParity);
        serial[num].setStopBits(QSerialPort::OneStop);
        serial[num].setFlowControl(QSerialPort::NoFlowControl);
        serial[num].setDataBits(QSerialPort::Data8);
        return num;
       }else{
        //somethingwrong= true;
        return -1;
    }
}


void MainWindow::on_connect_novasom_pushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Please connect NOVAsom to the workstation with RS232 cable.");
    msgBox.setWindowTitle("Checking NOVAsom.");
    msgBox.exec();
    disablepanel();
    int i =0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
         if(initPort(i,info)!=-1){
               i++;
         }else{
         }
    }

    if(i==0){
        QMessageBox msgBox1;
            msgBox1.setText("Unable to open SerialPort.\nNo NOVAsom found.");
            msgBox1.setWindowTitle("Checking NOVAsom.");
            msgBox1.exec();
        update_status_bar("No NOVAsom found!");

        return;
    }
    ui->connect_novasom_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(true);
    ui->runtest_pushButton->setEnabled(true);
    QMessageBox msgBox2;
    QString msg;
    char ms[5];
    itoa(i,ms,10);
    msg.append(ms);
    msg.append(" Novasom found.");
    msgBox2.setText(msg);
    msgBox2.setWindowTitle("Checking NOVAsom.");
    msgBox2.exec();
    connectedNOVAsom=i;
    ui->line_comboBox->setEnabled(false);
    ui->model_comboBox->setEnabled(false);

    QString messaggio= "NOVAsom trovati : ";
    char ciao[10];
    itoa(i,ciao,10);

    update_status_bar(messaggio+ciao);
    msgBox.setText("Please insert microSD card into NOVAsom, then power on the boards and wait splash screen.");
    msgBox.setWindowTitle("Checking NOVAsom.");
    msgBox.exec();
 return;
}

void MainWindow::on_runtest_pushButton_clicked()
{
    ui->runtest_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(false);

    QFile fl(lan + "WebServerResult.txt");
    if(fl.exists()){
        qDebug() << "esisteeeeeeeeeee222222";
        while(fl.exists()){
             qDebug() << "2222222esisteeeeeeeeeee222222";
            fl.remove();
        }
        qDebug() << "remoooveeeeee2222222";
    }

    QEventLoop loop;

    for (int i=0;i<connectedNOVAsom;i++){
        if (serial[i].isOpen()){
            connect(ui->NextPort_pushButton,&QPushButton::pressed,&loop,&QEventLoop::quit);
            serialport=&serial[i];
            testingNovasom(serialport);
            ui->GenerateParameters_PushButton->setEnabled(true);
            ui->NextPort_pushButton->setEnabled(false);
            loop.exec();
            finisciprocedurasingoloNOVASOM(serialport);
        }else {
        }
    }

    QMessageBox msgBox;
        msgBox.setText("Test complete!");
        msgBox.setWindowTitle("Test complete!");
        msgBox.exec();

    ui->GenerateParameters_PushButton->setEnabled(false);
    ui->NextPort_pushButton->setEnabled(false);
    ui->Write_EEprom_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(true);
    ui->readDataMatrix->setEnabled(false);
    ui->Print_Label_pushButton->setEnabled(false);

}

int MainWindow::testingNovasom(QSerialPort *serialp){

    QByteArray buffer;
    QByteArray line;
    QString filename;

    line.append("echo \"REFERENCE_SERVER="+ip+"\" > /etc/sysconfig/system_vars\n");
    line.append(". /etc/sysconfig/system_vars\n");
    serialp->write(line);

    filename=ui->line_comboBox->currentText()+"_"+ui->model_comboBox->currentText();
    if (filename.isEmpty()){
        return -1;
        }
    QFile file(lan+filename+".sh");
    QFile file1(lan+"WebServerResult.txt");

    if(file1.exists()){
        if(file1.remove()){
        }else{
            QMessageBox msgBox;
                msgBox.setText("[Error!]");
                msgBox.setWindowTitle("FileServerError-non sono riuscito ad eliminare il file risultato");
                msgBox.exec();
                return -1;
        }
    }else{
    }

    if(file.exists()){
       //do nothing



    }else{

        //il file non esiste!lo compilo!//non implementato per questa versione!!

    }
    serialp->write("\ncd /tmp;rm test.sh\n");
    QString tmp;
    tmp="wget http://"+ip+"/"+ filename +".sh\n";
    buffer.clear();
    buffer=tmp.toUtf8();
    serialp->write(buffer);
    serialp->write("\n");
    tmp.clear();
    buffer.clear();
    tmp="mv ./"+filename+".sh ./test.sh\n";
    buffer=tmp.toUtf8();
    serialp->write(buffer);
    serialp->write("\n");
    serialp->write("dos2unix ./test.sh\nchmod 777 test.sh\n");
    serialp->write("touch report.txt\n");
    serialp->write("./test.sh >> report.txt\n");


    return 0;
}

int MainWindow::finisciprocedurasingoloNOVASOM(QSerialPort *serialp){

    return 0;

}

void MainWindow::on_GenerateParameters_PushButton_clicked()
{
    int k;
    int escape;
    escape=0;

    do{
        escape++;
        k=checkResult();
        if(escape==5){
            //nonho trovato la rete per 5 volte?? forse problema cpu?
            QMessageBox err;
            err.setText("ERROR");
            err.setIcon(QMessageBox::Critical);
            err.setWindowTitle("ERROR");
            err.setInformativeText("NOVAsom unreacheble! Problem may be network or cpu...");
            //lastupdate
            serialport->write("\n\nreboot\n");
            err.exec();

            ui->GenerateParameters_PushButton->setEnabled(false);
            on_disconnect_novasom_pushButton_clicked();
            return;
        }
    }while(k==-2);

    QByteArray tmp;
    tmp.append("unix2dos report.txt ; ftpput --username novasis --password novasis ");
    tmp.append(ip);
    tmp.append(" report.txt \n\n ");

    if(k==0){
        //test pass
        update_status_bar("TEST PASS!!");
        ui->GenerateParameters_PushButton->setEnabled(false);
        ui->Write_EEprom_pushButton->setEnabled(true);
        QMessageBox msgBox;

        msgBox.setText("Do you want to generate parameters?");
        msgBox.setInformativeText("??");
        msgBox.setWindowTitle("??");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int video = msgBox.exec();
        QFile fl(lan + "report.txt");
        switch(video){
                case QMessageBox::Yes:

                    if(fl.exists()){
                        qDebug() << "esisteeeeeeeeeee";
                        while(fl.exists()){
                            fl.remove();
                        }
                        qDebug() << "remoooveeeeee";
                    }
                    serialport->write(tmp);
                    generateParameters();
                    break;
                case QMessageBox::No:
                    ui->NextPort_pushButton->setEnabled(true);
                    ui->Write_EEprom_pushButton->setEnabled(false);
                    break;
                default:
                    //should never be reached
                    break;
                }

    }else if(k==1){
        //test fail
        update_status_bar("TEST FAIL!!");
        ui->MAC_lineEdit->clear();
        ui->PN_lineEdit->clear();
        ui->Lotto_lineEdit->clear();
        ui->seriale_lineEdit->clear();
        ui->GenerateParameters_PushButton->setEnabled(false);
        ui->NextPort_pushButton->setEnabled(true);
        ui->Write_EEprom_pushButton->setEnabled(false);
        QString titolo;
        char stdstr[20];
        QByteArray line;
        int error;
        QFile fileerror(Fail+"error.txt");
        if(fileerror.exists()){
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
            line=fileerror.readAll();
            error=atoi(line);
            line.clear();

        }else{

            //crea file!!
            error=-1;
            line.clear();
            //clearstdstr(stdstr);
        }
            fileerror.close();
         error++;
         titolo.clear();
        itoa(error,stdstr,10);
        titolo.append(stdstr);
        clearstdstr(stdstr);


        if(!fileerror.isOpen()){
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
        }
        itoa(error,stdstr,10);
        fileerror.write(stdstr);
        clearstdstr(stdstr);
         fileerror.close();

    }else{
        //errore!!
    }


//adfgsghgfds//

}

int MainWindow:: checkResult(){
    QByteArray linebytearray;
    QFile file1(lan+"WebServerResult.txt");
    if(file1.exists()){
        //il risultato del test esiste!!

        if (!file1.open(QIODevice::ReadWrite|QIODevice::Text)){
            QMessageBox::information(this, tr("Unable to open file"),file1.errorString());
            qDebug() << "3";
            return -1;
           }
       linebytearray=file1.readAll();
       QMessageBox err;
       err.setText("TEST FAIL");
       err.setIcon(QMessageBox::Critical);
       err.setWindowTitle("Test Fail");
       err.setInformativeText("Test Fail");
       if(linebytearray.toStdString()=="OK"){

           QMessageBox msgBox;

               msgBox.setText("Check video...");
               msgBox.setInformativeText("Can you see video?");
               msgBox.setWindowTitle("Check result...");
               msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
               int video = msgBox.exec();
               switch(video){
               case QMessageBox::Yes:
                   ui->testpass_checkBox->setChecked(true);
                   ui->testFail_checkBox->setChecked(false);
                   return 0;
                   break;

               case QMessageBox::No:
                   ui->testpass_checkBox->setChecked(false);
                   ui->testFail_checkBox->setChecked(true);
                   err.exec();
                   return 1;
                   break;
               default:
                   //should never be reached
                   break;
               }
           return 0;
       }else if (linebytearray.toStdString()=="KO") {
           //Test Fail!!
           QMessageBox msgBox1;
               msgBox1.setText("Test Fail.");
               msgBox1.setWindowTitle("Check result...");
               msgBox1.exec();
               err.exec();
           ui->testpass_checkBox->setChecked(false);
           ui->testFail_checkBox->setChecked(true);
           return 1;
       }else{
           //test ????
           qDebug() << "TEST booooooooooooooooo!";

           QMessageBox msgBox2;
               msgBox2.setText("Problema sconosciuto...\nFile risultato non corretto.Ripetere il test...");
               msgBox2.setWindowTitle("???");
               msgBox2.exec();
           ui->testpass_checkBox->setChecked(false);
           ui->testFail_checkBox->setChecked(false);
            return -1;
       }

    }else{
        //il risultato del test non esiste!!!Problemi sul webserver???
        QMessageBox msgBox3;
            msgBox3.setText("WebServer problem...No network?? ");
            msgBox3.setWindowTitle("Check result...");
            msgBox3.exec();
        qDebug()<<"Risultato non trovato...Problema di rete??";
        return -2;
    }
}

void MainWindow::generateParameters(){

    QString filename,pn,lotto;
    filename=(ConfigDir+"/PN.ini");
    /*
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
        QString strKeyFunc("PN/");
        QSettings * settings = 0;
        settings = new QSettings( filename, QSettings::IniFormat );
        pn = getvalue(strKeyFunc, settings , ui->model_comboBox->currentText());
    }
    file.close();
    */
    pn=ui->model_comboBox->currentText();
    ui->PN_lineEdit->setText(pn);
    int week,year;
    char buffer[5];
    QDate date;
    if(date.currentDate().isValid()){
        week=date.currentDate().weekNumber();
        year=date.currentDate().year();
        if(ui->RM_checkBox->isChecked()){
            lotto.append("B");
        }else{
            lotto.append("A");
        }
        year=year-2000;
        itoa(year,buffer,10);
        lotto.append(buffer);
        itoa(week,buffer,10);
        lotto.append(buffer);
        //lotto.append("-");

        ui->Lotto_lineEdit->setText(lotto);
       }else{
        //non ho data!! sono fottuto!!
    }
    if(ui->RM_checkBox->isChecked()){
        filename=LogDir+pn+"_"+lotto+".ini";
        QFile file2(filename);
        QByteArray riga;
        char serialStr[6], buffer[7];
        int seriale, len,i;
        if(file2.exists()){
            //continua con l'ultimo seriale;
            file2.open(QIODevice::ReadOnly | QIODevice::Text);
            riga=file2.readAll();
            file2.close();
            seriale=atoi(riga);
            seriale=seriale+1;
            itoa(seriale,serialStr,10);
            len=strlen(serialStr);
            if(len<5){
                for(i=0;i<5;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<5-len;i++){

                    serialStr[i]='0';
                }

                strcat(serialStr,itoa(seriale,buffer,10));
            }
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }else{
            //crea file e ricomincia seriale
            strcpy(serialStr,"00001");
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }
        //ui->SERIALE_textBrowser->setText(serialStr);
        ui->seriale_lineEdit->setText(serialStr);



    }else{
        //ProgressiveMODE
        //apri file, leggi ultimo seriale scritto, prendi successivo!!
        filename=LogDir+pn+".ini";
        QFile file3(filename);
        char serialStr[7];
        QByteArray riga;
        char buffer[7];
        int seriale, len,i;
        if(file3.exists()){
            //il file esiste, leggi ultimo seriale, incrementa, scrivilo
            file3.open(QIODevice::ReadOnly | QIODevice::Text);
            riga=file3.readAll();
            file3.close();
            seriale=atoi(riga);
            seriale=seriale+1;
            itoa(seriale,serialStr,10);
            len=strlen(serialStr);

            if(len<6){
                for(i=0;i<6;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<6-len;i++){

                    serialStr[i]='0';
                }

                strcat(serialStr,itoa(seriale,buffer,10));
            }
            file3.open(QIODevice::WriteOnly | QIODevice::Text);
            file3.write(serialStr);
            file3.close();
        }else{

            //crea file!!
            strcpy(serialStr,"000001");
            file3.open(QIODevice::WriteOnly | QIODevice::Text);
            file3.write(serialStr);
            file3.close();
        }
        //ui->SERIALE_textBrowser->setText(serialStr);
        ui->seriale_lineEdit->setText(serialStr);
    }

    //MAC

    char stdstr[20];
    filename=MACdir+pn+".ini";
    QFile file4(filename);
    QByteArray line;
    QString mac;
    int macseriale;
    int l;

    if(file4.exists()){
                //il file esiste, leggi ultimo seriale, incrementa, scrivilo
                clearstdstr(stdstr);
                file4.open(QIODevice::ReadWrite | QIODevice::Text);
                line=file4.readAll();
                macseriale=atoi(line);
                line.clear();
                //itoa(macseriale,stdstr,10);

            }else{

                //crea file!!
                macseriale=-1;
                line.clear();
                clearstdstr(stdstr);
                //itoa(macseriale,stdstr,10);
                //file4.open(QIODevice::ReadWrite | QIODevice::Text);
                //file4.write(stdstr);
            }
    file4.close();

    macseriale++;
    itoa(macseriale,stdstr,10);

    if(!file4.isOpen()){
        file4.open(QIODevice::ReadWrite | QIODevice::Text);
    }

    file4.write(stdstr);
    clearstdstr(stdstr);

    itoa(macseriale,stdstr,16);

    file4.close();

    mac="00:4e:";
    mac.append(pn[11]);
    mac.append(pn[12]);
    mac.append(':');
    mac.append(pn[15]);

    l=strlen(stdstr);



    if(l<5){
        mac.append("0:");
        if(l<4){
            mac.append('0');
            if(l<3){

                mac.append("0:");
                if(l<2){

                    mac.append('0');
                    mac.append(stdstr[0]);
                }else{
                    mac.append(stdstr[0]);
                    mac.append(stdstr[1]);
                }
            }else{

                mac.append(stdstr[0]);
                mac.append(':');
                mac.append(stdstr[1]);
                mac.append(stdstr[2]);
            }
        }else{
            mac.append(stdstr[0]);
            mac.append(stdstr[1]);
            mac.append(':');
            mac.append(stdstr[2]);
            mac.append(stdstr[3]);
        }
    }else{
        //e se è maggiore di 5?? so cazzi!!
        mac.append(stdstr[0]);
        mac.append(':');
        mac.append(stdstr[1]);
        mac.append(stdstr[2]);
        mac.append(':');
        mac.append(stdstr[3]);
        mac.append(stdstr[4]);
    }
    ui->MAC_lineEdit->setText(mac);
}

void MainWindow::on_Write_EEprom_pushButton_clicked()
{
    if(ui->testpass_checkBox->isChecked()){
        if(ui->line_comboBox->currentText()=="P-Line"){
            QString pn,mac,lotto,seriale;
            serialport->write("get_exec e2info\n");

            pn=ui->PN_lineEdit->text();
            qDebug() << pn.toUtf8();
            seriale=ui->seriale_lineEdit->text();
            qDebug() << seriale.toUtf8();
            lotto=ui->Lotto_lineEdit->text();
            qDebug() << lotto.toUtf8();
            mac=ui->MAC_lineEdit->text();
            qDebug() << mac.toUtf8();
            serialport->write("chmod 777 e2info\n");
            serialport->write("./e2info -P "+ pn.toUtf8()+"\n");
            serialport->write("./e2info -M "+ mac.toUtf8()+"\n");
            serialport->write("./e2info -S "+ seriale.toUtf8()+"\n");
            serialport->write("./e2info -L "+ lotto.toUtf8()+"\n");
        }
        QString titolo;

        titolo.append(ui->PN_lineEdit->text());
        titolo.append("_");
        titolo.append(ui->seriale_lineEdit->text());
        titolo.append("_");
        titolo.append(ui->Lotto_lineEdit->text());
        generateReport(titolo);

        ui->NextPort_pushButton->setEnabled(true);
        ui->Write_EEprom_pushButton->setEnabled(false);
        ui->Print_Label_pushButton->setEnabled(true);


    }else{
        //se il test è fallito??//i
        /*
        QString titolo;
        char stdstr[20];
        QByteArray line;
        int error;
        QFile fileerror(Fail+"error.txt");
        if(fileerror.exists()){
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
            line=fileerror.readAll();
            error=atoi(line);
            line.clear();

        }else{

            //crea file!!
            error=-1;
            line.clear();
            //clearstdstr(stdstr);
        }
            fileerror.close();
         error++;
         titolo.clear();
        itoa(error,stdstr,10);
        titolo.append(stdstr);
        clearstdstr(stdstr);


        if(!fileerror.isOpen()){
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
        }
        itoa(error,stdstr,10);
        fileerror.write(stdstr);
        clearstdstr(stdstr);
        generateReport(titolo);
        */

        ui->NextPort_pushButton->setEnabled(true);
        ui->Write_EEprom_pushButton->setEnabled(false);
        ui->Print_Label_pushButton->setEnabled(false);

    }

    ui->readDataMatrix->setEnabled(true);
}

int MainWindow::generateReport(QString filename){


    QFile report(directory+filename+".txt");
    qDebug()<< filename;

    if(!report.open(QIODevice::ReadWrite|QIODevice::Text)){
       qDebug() << "Non sono riuscito a creare il file1!";
       return -1;
        }


    QFile original(lan + "report.txt");


    if(!original.open(QIODevice::ReadOnly|QIODevice::Text)){
       qDebug() << "Non sono riuscito ad aprire il file2!";
       return -1;
        }
    QByteArray rep;

    rep=original.readAll();
    qDebug() << rep;

    report.write("#");
    report.write(filename.toUtf8());
    report.write("\n");
    report.write("#");
    report.write(user.toUtf8());
    report.write("\n");







    if(ui->testpass_checkBox->isChecked()){

        report.write("#TEST PASS!\n");
    }else{
        report.write("#TEST FAIL!");
    }

    report.write("\nPN: ");
    report.write(ui->PN_lineEdit->text().toUtf8());
    report.write("\nSeriale: ");
    report.write(ui->seriale_lineEdit->text().toUtf8());
    report.write("\nLotto: ");
    report.write(ui->Lotto_lineEdit->text().toUtf8());
    report.write("\nMAC ADDRESS: ");
    report.write(ui->MAC_lineEdit->text().toUtf8());
    report.write("\n");




    report.write(rep);
    report.close();
    createPDF(filename);
    return 0;
}

int MainWindow:: createPDF(QString filename){

    QFile file(directory+filename+".txt");

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return -1;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(directory+filename+"report.pdf");

    QPainter painter;
    painter.begin(&printer);
    QString line;
    int x,y;
    x=20;
    y=30;
    while (!file.atEnd()) {
           line = file.readLine();
           painter.drawText(x,y,line);
           line.clear();
           y=y+120;
       }
    file.close();
    painter.end();
    return 0;
}

void MainWindow::on_Print_Label_pushButton_clicked()
{
    QString titolo;

    titolo.append(ui->PN_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->seriale_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->Lotto_lineEdit->text());
    titolo.append("_");
    titolo.append("label.txt");

    generateLabel(titolo);
    sleep(1);
    printLabel(titolo);

    ui->Print_Label_pushButton->setEnabled(false);
}

int MainWindow::generateLabel(QString filename){


    QFile source(labeltemplate);
    QFile destination(directory+filename);
    QFile destination1(directory+"last_label.txt");
    int len;


    if (!source.open(QIODevice::ReadOnly | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta!Contatta gestore sw!");
        return -1;
    }else{
    }

    QByteArray text;
    char *textstring;

    text=source.readAll();

    len=text.count();
    textstring=(char *)malloc(sizeof(char)*(len+1));
    if(textstring==NULL){
        update_status_bar("MEMORY ERROR!Impossibile generare etichetta!Contatta gestore sw!");
        return -1;
    }
    strcpy(textstring,text.constData());

    text.clear();
    int count;
    count=0;
    for(int i=0; i<len;i++){
        if(textstring[i]!='$'){
            text.append(textstring[i]);
        }else{
            if(count == 0){
                //quanti '$' ci sono??
                text.append('$');
            }
            if(count== 1){
                //tutto
                text.append(ui->PN_lineEdit->text());
                text.append(ui->Lotto_lineEdit->text());
                text.append(ui->seriale_lineEdit->text());
                //qDebug()
            }
            if(count == 2){
                //PN
                text.append(ui->PN_lineEdit->text());
            }
            if(count == 3){
                //DC
                text.append(ui->Lotto_lineEdit->text());
            }
            if(count == 4){
                //SN
                text.append(ui->seriale_lineEdit->text());
            }
            if(count == 5){
                //quanti '$' ci sono??
                text.append('$');
            }
            count ++;
        }
    }

    if (!destination.open(QIODevice::ReadWrite | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta2!Contatta gestore sw!");
        return -1;
    }else{
    }
    if (!destination1.open(QIODevice::ReadWrite | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta2!Contatta gestore sw!");
        return -1;
    }else{

    }

    destination.write(text);
    destination.close();
    destination1.write(text);
    destination1.close();
    return 0;
}

int MainWindow::printLabel(QString filename)
{
    system("notepad \/p \/NOVATEST_SP\/TESTED\/last_label.txt");

    return 0;
}


void MainWindow::on_PM_checkBox_clicked()
{
    if(ui->PM_checkBox->isChecked()){
        ui->RM_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->RM_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_RM_checkBox_clicked()
{
    if(ui->RM_checkBox->isChecked()){
        ui->PM_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->PM_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_testpass_checkBox_clicked()
{
    if(ui->testpass_checkBox->isChecked()){
        ui->testFail_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->testFail_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_testFail_checkBox_clicked()
{
    if(ui->testFail_checkBox->isChecked()){
        ui->testpass_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->testpass_checkBox->setCheckState(Qt::Checked);
    }
}


void MainWindow::on_load_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Configuration File"), "D:/QtProjects/NOVATest",tr("Configuration Files (*.ini)"));
    if (fileName.isEmpty())
    {
        qDebug() << "fileName is empty";
        return;
    }
    else
    {
        qDebug() << "fileName :"+fileName;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            qDebug() << "1";
            return;
        }
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;

            QFileInfo fi(fileName);
            settings = new QSettings( fileName, QSettings::IniFormat );
            if ( getvalue(strKeyFunc, settings , "audio_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->audio_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->audio_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "bt_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->bt_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->bt_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "console_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->console_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->console_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "cpu_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->cpu_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->cpu_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eeprom_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eeprom_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eeprom_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eMMC_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eMMC_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eMMC_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eth_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eth_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eth_checkBox->setChecked(false);
                }

            if ( getvalue(strKeyFunc, settings , "hdmi_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->hdmi_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->hdmi_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "ram_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->ram_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->ram_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "rtc_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->rtc_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->rtc_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "sdcard_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->sdcard_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->sdcard_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "sensors_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->sensors_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->sensors_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "touch_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->touch_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->touch_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "usb_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->usb_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->usb_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "wifi_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->wifi_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->wifi_checkBox->setChecked(false);
                }
            file.close();
    }
}

void MainWindow::on_save_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Configuration File"), "D:/QtProjects/",tr("Configuration Files (*.ini)"));

    if (fileName.isEmpty())
    {
        qDebug() << "fileName is empty";
        return;
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            qDebug() << "5";
            return;
        }
        QTextStream out(&file);
        out << QString("[CONFIG]\r\n");
        if ( ui->audio_checkBox->isChecked() )
            out << QString("audio_checkBox=true\r\n");
        else
            out << QString("audio_checkBox=false\r\n");
        if ( ui->bt_checkBox->isChecked() )
            out << QString("bt_checkBox=true\r\n");
        else
            out << QString("bt_checkBox=false\r\n");
        if ( ui->console_checkBox->isChecked() )
            out << QString("console_checkBox=true\r\n");
        else
            out << QString("console_checkBox=false\r\n");
        if ( ui->cpu_checkBox->isChecked() )
            out << QString("cpu_checkBox=true\r\n");
        else
            out << QString("cpu_checkBox=false\r\n");
        if ( ui->eeprom_checkBox->isChecked() )
            out << QString("eeprom_checkBox=true\r\n");
        else
            out << QString("eeprom_checkBox=false\r\n");
        if ( ui->eMMC_checkBox->isChecked() )
            out << QString("eMMC_checkBox=true\r\n");
        else
            out << QString("eMMC_checkBox=false\r\n");
        if ( ui->eth_checkBox->isChecked() )
            out << QString("eth_checkBox=true\r\n");
        else
            out << QString("eth_checkBox=false\r\n");
        if ( ui->hdmi_checkBox->isChecked() )
            out << QString("hdmi_checkBox=true\r\n");
        else
            out << QString("hdmi_checkBox=false\r\n");
        if ( ui->ram_checkBox->isChecked() )
            out << QString("ram_checkBox=true\r\n");
        else
            out << QString("ram_checkBox=false\r\n");
        if ( ui->rtc_checkBox->isChecked() )
            out << QString("rtc_checkBox=true\r\n");
        else
            out << QString("rtc_checkBox=false\r\n");
        if ( ui->sdcard_checkBox->isChecked() )
            out << QString("sdcard_checkBox=true\r\n");
        else
            out << QString("sdcard_checkBox=false\r\n");
        if ( ui->sensors_checkBox->isChecked() )
            out << QString("sensors_checkBox=true\r\n");
        else
            out << QString("sensors_checkBox=false\r\n");
        if ( ui->touch_checkBox->isChecked() )
            out << QString("touch_checkBox=true\r\n");
        else
            out << QString("touch_checkBox=false\r\n");
        if ( ui->usb_checkBox->isChecked() )
            out << QString("usb_checkBox=true\r\n");
        else
            out << QString("usb_checkBox=false\r\n");
        if ( ui->wifi_checkBox->isChecked() )
            out << QString("wifi_checkBox=true\r\n");
        else
            out << QString("wifi_checkBox=false\r\n");
        file.close();
    }
}

void MainWindow::on_NextPort_pushButton_clicked()
{
    ui->Print_Label_pushButton->setEnabled(false);
    //ui->readDataMatrix->setEnabled(false);
    ui->readDataMatrix->setDisabled(true);
}



void MainWindow::on_readDataMatrix_clicked()
{
    DialogDataMatrix *d = new DialogDataMatrix(this);
    d->show();
    ui->readDataMatrix->setEnabled(false);

    //UsrPswDialog *p = new UsrPswDialog(this);
    //p->show();
}
