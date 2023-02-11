#include "tcpconnection.h"
#include <QHostAddress>

TcpConnection::TcpConnection(QObject *parent): QObject{parent}, socket_(new QTcpSocket(this))
{
}

TcpConnection::~TcpConnection()
{
    if(socket_)
    {
        socket_->disconnectFromHost();
        socket_->waitForDisconnected();
        socket_->close();
        delete socket_;
    }
}

void TcpConnection::connectHost(const QString &ip, const quint16 &port)
{
    this->serverIP_ = ip;
    this->serverPort_ = port;
    QHostAddress addr = QHostAddress(ip);
    socket_->connectToHost(addr, port);
    connect(socket_, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket_, SIGNAL(errorOccurred), this, SLOT(errorOcc));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(disConnected()));
    connect(socket_, SIGNAL(readyRead()), this, SLOT(msgFromServer()));
}


void TcpConnection::disConnect() const
{
    if(socket_)
    {
        socket_->disconnectFromHost();
        socket_->waitForDisconnected();
    }
}

void TcpConnection::setUserName(const QString &name)
{
    this->userName = name;
}

QString TcpConnection::getUserName() const
{
    return this->userName;
}

Message TcpConnection::getNewestMsg() const
{
    return this->messageList_.back();
}

std::vector<Message> TcpConnection::getMsgList() const
{
    return this->messageList_;
}

void TcpConnection::addMessageList(const QString &time, const QString &sender, const QString &receiver, const QString &content)
{
    Message msg = {sender, receiver, content, time};
    this->messageList_.emplace_back(msg);
}

void TcpConnection::sendMessage(const QByteArray &msg) const
{
    qDebug()<<"SEND: "<<msg;
    socket_->write(msg);
    socket_->waitForBytesWritten();
    //socket_->flush();
}

QTcpSocket* TcpConnection::getSocket() const
{
    return socket_;
}

QStringList  TcpConnection::getMemberList() const
{
    return this->memberList_;
}

FileInfo  TcpConnection::getFileInfo() const
{
    return this->fileinfo_;
}

void TcpConnection::connected() const
{
    qDebug()<<"client connected server!";
}

void TcpConnection::disConnected() const
{
    qDebug()<<"client disConnected server!";
}

void TcpConnection::errorOcc() const
{
    qDebug()<<"error occurred!";
}

void TcpConnection::msgFromServer()
{
    QByteArray message = socket_->readAll();
    QByteArrayList subMessage = message.split('}');

    foreach (QByteArray subm, subMessage)
    {
        subm += '}';
        QJsonParseError err_rpt;
        QJsonDocument  doucment = QJsonDocument::fromJson(subm, &err_rpt);
        if(err_rpt.error == QJsonParseError::NoError)
        {
            qDebug()<<"RECEIVE: "<<subm.data();
            QJsonObject object = doucment.object();
            if(object.contains("label"))
            {
                QJsonValue label = object.value("label");
                if(label == "message")//聊天信息
                {
                    Message newMsg;
                    if (object.contains("content"))
                    {
                        newMsg.content = object.value("content").toString();
                    }
                    if (object.contains("sender"))
                    {
                        newMsg.sender = object.value("sender").toString();
                    }
                    if (object.contains("time"))
                    {
                        newMsg.time = object.value("time").toString();
                    }
                    if (object.contains("receiver"))
                    {
                        newMsg.receiver = object.value("receiver").toString();
                    }
                    this->messageList_.emplace_back(newMsg);//加入信息队列中通知chatwindow处理
                    emit   receiveMsg();
                }
                else if(label == "sign in")//登录信息
                {
                    if (object.contains("content"))
                    {
                        QString content = object.value("content").toString();
                        if(content =="success" && object.contains("name"))
                        {
                            this->setUserName(object.value("name").toString());
                        }
                        else if(content == "wrong id or password")
                        {
                            if(raiseMessageBox)
                            {
                                raiseMessageBox("提示","账号或密码错误");
                            }
                        }
                        else if(content == "repeat sign in")
                        {
                            if(raiseMessageBox)
                            {
                                raiseMessageBox("提示","账号已登录");
                            }
                        }
                    }
                }
                else if(label == "sign up")//注册信息
                {
                    if (object.contains("content"))
                    {
                        QString content = object.value("content").toString();
                        if(content =="success" && object.contains("name"))
                        {
                            this->setUserName(object.value("name").toString());
                        }
                        else if(content=="repeat id")
                        {
                            if(raiseMessageBox)
                            {
                                raiseMessageBox("提示","账号已存在");
                            }
                        }
                    }
                }
                else if(label == "member")//聊天室成员信息
                {
                    if (object.contains("content"))
                    {
                        QStringList people = object.value("content").toString().split(' ');
                        memberList_.clear();
                        for(const auto & p : people)
                        {
                            if(!p.isEmpty() && p != this->userName)
                            {
                                memberList_.append(p);
                            }
                        }
                    }
                    emit memberChange();//通知chatwindow处理
                }
                else if(label == "file")//文件传输信息
                {
                    if (object.contains("sender"))
                    {
                        this->fileinfo_.sender = object.value("sender").toString();
                    }
                    if (object.contains("ip"))
                    {
                        this->fileinfo_.ip = object.value("ip").toString();
                    }
                    if (object.contains("port"))
                    {
                        this->fileinfo_.port = object.value("port").toInt();
                    }
                    if (object.contains("file name"))
                    {
                        this->fileinfo_.fileName = object.value("file name").toString();
                    }
                    if (object.contains("file size"))
                    {
                        this->fileinfo_.fileSize = object.value("file size").toInt();
                    }
                    emit fileRecive();//通知chatwindow处理
                }
            }
        }
    }
}

void TcpConnection::setRaiseMessageBox(std::function<void(QString, QString)> const &mbox)
{
    this->raiseMessageBox = std::move(mbox);
}
