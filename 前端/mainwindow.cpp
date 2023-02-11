#include "mainwindow.h"
#include "ui_mainwindow.h"


const int ID_LENGTH_LIMIT = 20;
const int PWD_LENGTH_LIMIT = 20;
const int CONNECT_WAIT_TIME = 2;

MainWindow::MainWindow(QWidget *parent, TcpConnection *_conn)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Fake Chat");
    this->setStyleSheet("QMainWindow {background-color:rgb(255, 255, 255)}");

    ui->signinBut->setStyleSheet("QPushButton{background-color: rgb(8, 255, 247) ;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                              "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                              "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->signupBut->setStyleSheet("QPushButton{background-color: rgb(255, 255, 255) ;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                 "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                 "QPushButton:pressed{background-color:rgb(219, 231, 255);border-style: inset;}");

    this->conn = _conn;
    this->chatwindow = new ChatWindow(nullptr, this->conn);
    ui->password->setEchoMode(QLineEdit::Password);
    //注册弹出窗口的回调函数
    std::function<void(QString, QString)> cb = std::bind(&MainWindow::raiseMessageBox, this, std::placeholders::_1, std::placeholders::_2);
    this->conn->setRaiseMessageBox(cb);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_signinBut_clicked()
{
    this->conn->setUserName("");
    //this->chatwindow->show();
    //this->hide();
    QString userid = ui->userID->text();
    QString passwd = ui->password->text();
    QString passwd_md5 = QString(QCryptographicHash::hash (passwd.toUtf8(), QCryptographicHash::Md5));//md5加密

    if(userid.isEmpty())
    {
        QMessageBox::information(this, "提示","请输入账号");
        return;
    }
    if(userid.size()>ID_LENGTH_LIMIT)
    {
        QMessageBox::information(this, "提示",tr("账号长度应小于%1个字符").arg(ID_LENGTH_LIMIT));
        return;
    }
    if(userid.contains(" "))
    {
        QMessageBox::information(this, "提示",tr("账号不应该包含空格"));
        return;
    }
    if(passwd.isEmpty())
    {
        QMessageBox::information(this, "提示","请输入密码");
        return;
    }
    if(passwd.size()>ID_LENGTH_LIMIT)
    {
        QMessageBox::information(this, "提示",tr("密码长度应小于%1个字符").arg(PWD_LENGTH_LIMIT));
        return;
    }
    if(passwd.contains(" "))
    {
        QMessageBox::information(this, "提示",tr("密码不应该包含空格"));
        return;
    }

    if(this->conn)
    {
        QJsonObject json;
        json.insert("id",userid);
        json.insert("password",passwd_md5);
        json.insert("label","sign in");
        QJsonDocument doc(json);
        this->conn->sendMessage(doc.toJson());

        if(this->conn->getSocket()->waitForReadyRead(CONNECT_WAIT_TIME * 1000))//等待服务器回复
        {
            if(!this->conn->getUserName().isEmpty())
            {
                QMessageBox::information(this, "提示",tr("登录成功"));
                this->chatwindow->show();
                this->hide();
                return;
            }
        }
        else
            QMessageBox::information(this, "提示",tr("请检查网络设置！"));
    }

}

void MainWindow::on_signupBut_clicked()
{
    QString userid = ui->userID->text();
    QString passwd = ui->password->text();
    QString passwd_md5 = QString(QCryptographicHash::hash (passwd.toUtf8(), QCryptographicHash::Md5));
    if(userid.isEmpty())
    {
        QMessageBox::information(this, "提示","请输入账号");
        return;
    }
    if(userid.size()>ID_LENGTH_LIMIT)
    {
        QMessageBox::information(this, "提示",tr("账号长度应小于%1个字符").arg(ID_LENGTH_LIMIT));
        return;
    }
    if(userid.contains(" "))
    {
        QMessageBox::information(this, "提示",tr("账号不应该包含空格"));
        return;
    }
    if(passwd.isEmpty())
    {
        QMessageBox::information(this, "提示","请输入密码");
        return;
    }
    if(passwd.size()>ID_LENGTH_LIMIT)
    {
        QMessageBox::information(this, "提示",tr("密码长度应小于%1个字符").arg(PWD_LENGTH_LIMIT));
        return;
    }
    if(passwd.contains(" "))
    {
        QMessageBox::information(this, "提示",tr("密码不应该包含空格"));
        return;
    }
    if(this->conn)
    {
        QJsonObject json;
        json.insert("id",userid);
        json.insert("password",passwd_md5);
        json.insert("label","sign up");
        QJsonDocument doc(json);
        this->conn->sendMessage(doc.toJson());

        if(this->conn->getSocket()->waitForReadyRead(CONNECT_WAIT_TIME * 1000))//等待服务器回复
        {
            if(!this->conn->getUserName().isEmpty())
            {
                QMessageBox::information(this, "提示",tr("注册成功"));
                return;
            }
        }
        else
            QMessageBox::information(this, "提示",tr("请检查网络设置！"));
    }
}

void MainWindow::raiseMessageBox(const QString &title, const QString &message)
{
    QMessageBox::information(this, title, message);
}

