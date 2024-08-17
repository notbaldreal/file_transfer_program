#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFile>
#include<QHostAddress>
#include<QFileDialog>
#include<QString>
#include<QByteArray>
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建套接字,服务器
    QTcpServer *server_tcp= new QTcpServer(this);
    QTcpSocket *sock_serv= new QTcpSocket(this);
    QTcpSocket *sock_cli= new QTcpSocket(this);

    //服务器端槽函数
    connect(server_tcp,&QTcpServer::newConnection,this,[=]()mutable{
        sock_serv = server_tcp->nextPendingConnection();
        ui->record_serv->appendPlainText(tr("some one has linked ! "));
    });
    connect(sock_serv,&QTcpSocket::readyRead,this,[=](){
        in = sock_serv->readAll();
        ui->record_serv->appendPlainText(tr("some file has been recv"));
    });
    connect(sock_serv,&QTcpSocket::disconnected,this,[=](){
        ui->record_serv->appendPlainText(tr("someone disconnected ! "));
        sock_serv->deleteLater();
    });
    //client端信号槽
    connect(sock_cli,&QTcpSocket::connected,this,[=](){
        QString s("succeed to connected ! ");
        ui->record_cli->setPlainText(s);
    });
    connect(sock_cli,&QTcpSocket::disconnected,this,[=](){
        ui->record_cli->setPlainText(tr("link already disconnected ! "));
        sock_cli->deleteLater();
    });
    // 进度条的信号槽
    connect(this,&MainWindow::progressUpdated,this,&MainWindow::updateProgressBar);
    qDebug()<<"constructor has done ! ";

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::init_progressbar(qint64 data)
{
    ui->processBar_client->setRange(0,data);
}

void MainWindow::on_selectFile_client_clicked()
{
    filePath_c = QFileDialog::getOpenFileName(this,tr("select which file u want to send "),
                                                    "C:/Users/asus/Pictures/rc",tr("Image Files (*.png *.jpg *.bmp)"));
    if(filePath_c.isEmpty()) {qDebug()<<"failed to open path";
        return ;
    }
    qDebug()<<"the filePath of file is " << filePath_c;
    file_data_c.setFileName(filePath_c);
    file_info_c.setFile(file_data_c);
    ui->file_path_cli->setText(filePath_c);
    ui->record_cli->setPlainText(tr("u have select file ! "));
    //初始化进度条
    init_progressbar(file_info_c.size());

}


void MainWindow::on_sendFile_client_clicked()
{

    quint64 port = ui->port_client->text().toInt();
    QString ip = ui->ip_client->text();
    qDebug()<<"pre local port is " << port;
    qDebug()<<"pre local ip is " << ip;
    if(!file_info_c.isFile()){
        QMessageBox::critical(this,tr("FILE ERROR"),
                              tr("there ar no file to transfer ! "));
        return;
    }
    qDebug()<<"finished the !file_info_c.isFile() section ! ";
    if(file_data_c.open(QIODevice::ReadOnly)){
        qDebug()<<"finished the  file_data_c.open section ! ";
        QHostAddress addr(ip);
        if(addr.isNull()){
            qDebug()<<"ip is not a valid";
            return;
        }

    sock_cli->connectToHost(addr,port);

    qDebug()<<"local port is " << sock_cli->localPort();
    qDebug()<<"local ip is " << sock_cli->localAddress().toString();
    if(sock_cli->waitForConnected()){
        while(!file_data_c.atEnd()){
            QByteArray chunk =file_data_c.read(chunkSize);
            sock_cli->write(chunk);
            if(sock_cli->waitForBytesWritten()){
                transferred_cli+=chunk.size();
                emit progressUpdated(transferred_cli);
            }else{
                break;
            }
        }
        file_data_c.close();
        sock_cli->close();
    }
    }else{
        qDebug()<<"file open failed ! ";
        return;
    };
}


void MainWindow::on_launch_server_clicked()
{
    quint64 port = ui->port_server->text().toUInt();
    qDebug()<<"the listenting prot has finished ! "<< port;
    if(!server_tcp->listen(QHostAddress::Any,port)){
        qDebug()<<"lsitening error ! ";
        return;
    }
    ui->record_serv->appendPlainText(tr("serv is listening ! "));
}

void MainWindow::updateProgressBar(qint64 data)
{
    ui->processBar_client->setValue(data);
}

