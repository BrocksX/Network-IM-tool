#include "mainwindow.h"
#include "tcpconnection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //新建TCP连接类，管理与服务器的通信
    TcpConnection *tcpconn = new TcpConnection();
    //信息长度要小于3000
    //192.168.112.128
    //127.0.0.1
    //192.168.112.1
    tcpconn->connectHost("192.168.112.128", 8888);
    MainWindow w(nullptr,tcpconn);
    w.show();
    return a.exec();
}
