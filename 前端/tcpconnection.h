#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>
#include <functional>
#include <vector>

struct Message
{
    QString    sender;
    QString    receiver;
    QString    content;
    QString    time;
};

struct FileInfo
{
    QString ip;
    quint16 port;
    QString sender;
    QString fileName;
    qint64  fileSize;
};

class TcpConnection : public QObject
{
    Q_OBJECT
public:
    explicit TcpConnection(QObject *parent = nullptr);
    ~TcpConnection();
    void     connectHost(const QString &ip, const quint16 &port);
    void     disConnect() const;
    void     sendMessage(const QByteArray &msg) const;
    QTcpSocket* getSocket() const;

    void     setUserName(const QString &name);
    QString  getUserName() const;
    Message  getNewestMsg() const;
    std::vector<Message> getMsgList() const;
    void     addMessageList(const QString &time, const QString &sender, const QString &receiver, const QString &content);
    QStringList  getMemberList() const;
    FileInfo    getFileInfo() const;

    //弹出信息窗口的回调函数
    void     setRaiseMessageBox(std::function<void(QString, QString)> const &mbox);


signals:
    void     receiveMsg();
    void     memberChange();
    void     fileRecive();

private slots:
    void     connected() const;
    void     disConnected() const;
    void     errorOcc() const;
    void     msgFromServer();

private:
    QTcpSocket*     socket_;
    QString         serverIP_;
    quint16        serverPort_;
    QString         userName;
    std::function<void(QString, QString)> raiseMessageBox;
    QStringList     memberList_;
    std::vector<Message>   messageList_;
    FileInfo   fileinfo_;

};
#endif // TCPCONNECTION_H
