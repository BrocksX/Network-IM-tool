#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include "tcpconnection.h"
#include "filetransfer.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr, TcpConnection *_conn = nullptr);
    ~ChatWindow();

private slots:
    //发送信息
    void on_sendmsgbut_clicked();
    //显示成员信息
    void on_listWidget_itemClicked(QListWidgetItem *item);
    //发送文件
    void on_sendfile_clicked();

    void receiveMsgCallback();
    void memberChangeCallback();
    void receiveFileCallback();
    void freeFileReceiver();


private:
    Ui::ChatWindow *ui;
    TcpConnection *conn;
    QString topChatwindow_;
    FileReceiver *receiver_;
};

#endif // CHATWINDOW_H
