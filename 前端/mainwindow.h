#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatwindow.h"
#include "tcpconnection.h"
#include <QMessageBox>
#include <QCryptographicHash>

//登录界面
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, TcpConnection *_conn = nullptr);
    ~MainWindow();

    void raiseMessageBox(const QString &title, const QString &message);

private slots:
    void on_signinBut_clicked();
    void on_signupBut_clicked();

private:
    Ui::MainWindow *ui;
    TcpConnection *conn;
    ChatWindow *chatwindow;
};
#endif // MAINWINDOW_H
