#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QFile>
#include<QFileInfo>
#include<QThread>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init_progressbar(qint64 data);
signals:
    void progressUpdated(qint64 data);
private slots:
    void on_selectFile_client_clicked();

    void on_sendFile_client_clicked();

    void on_launch_server_clicked();

    void updateProgressBar(qint64 data);
private:
    Ui::MainWindow *ui;
    QTcpServer *server_tcp;
    QTcpSocket *sock_serv;
    QTcpSocket *sock_cli;
    //client 端的文件路径,和文件对象
    QString filePath_c;
    QFile file_data_c;
    QFileInfo file_info_c;
    qint64 chunkSize = 1024;
    qint64 transferred_cli=0;
    qint64 transferred_serv=0;
    QByteArray in;

};
#endif // MAINWINDOW_H
