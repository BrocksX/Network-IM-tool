#include "filetransfer.h"

const int WRITE_BYTES = 4;
QString getLocalIPv4();

FileSender::FileSender(QObject *parent) :  QObject{parent}, server_(new QTcpServer(this)), socket_(nullptr)
{
}

void FileSender::listen(const unsigned &port)
{
    server_->listen(QHostAddress::Any, port);
    this->port_ = server_->serverPort();
    this->ip_ = getLocalIPv4();
    connect(server_, &QTcpServer::newConnection, [&]()
    {
        this->socket_ = server_->nextPendingConnection();
        qDebug()<<"connect to receiver";
    });

}

QTcpServer* FileSender::getServerSocket() const
{
    return this->server_;
}

bool FileSender::selectFile(QString path)
{
    if(path.isEmpty())
        return false;
    file_.setFileName(path);
    if(file_.open(QIODevice::ReadOnly))
    {
        QFileInfo info(path);
        fileName_= info.fileName();
        fileSize_ = info.size();
        qDebug()<<"select file finished";
        return true;
    }
    else
        return false;
}

bool FileSender::sendFile()
{
    if(!file_.exists() || fileName_.isEmpty() || fileSize_ == 0)
        return false;

    qint64 sentSize = 0;
    qint64 len = 0;
    qint64 sent = 0;
    char buf[WRITE_BYTES*1024] = {0};
    while(sentSize < fileSize_)
    {
        memset(&buf, 0, sizeof(buf));
        len = file_.read(buf, sizeof(buf));
        sent = socket_->write(buf, len);
        sentSize += sent;
        if(len <= 0 || sent <= 0)
            break;
    }
    if(sentSize == fileSize_)
    {
        qDebug()<<"send success";
        file_.close();
        socket_->disconnectFromHost();
        socket_->waitForDisconnected();
        socket_->close();
        return true;
    }
    else
    {
        qDebug()<<"send failed";
        file_.close();
        socket_->disconnectFromHost();
        socket_->waitForDisconnected();
        socket_->close();
        return false;
    }
}

QString FileSender::getIP() const
{
    return this->ip_;
}

quint16 FileSender::getPort() const
{
    return this->port_;
}

QString FileSender::getFileName() const
{
    return this->fileName_;
}

qint64 FileSender::getFileSize() const
{
    return this->fileSize_;
}
//--------------------------------------------------------------------------------------------------------

FileReceiver::FileReceiver(QObject *parent) :  QObject{parent}, socket_(new QTcpSocket(this))
{
}

void  FileReceiver::connectHost(const QString &ip, const quint16 &port)
{
    this->ip_ = ip;
    this->port_ = port;
    QHostAddress addr = QHostAddress(ip);
    socket_->connectToHost(addr, port);
    if(socket_->waitForConnected())
    {
        qDebug()<<"file receiver connected server!";
    }
    else
    {
        qDebug()<<tr("fail to connect %1 : %2").arg(ip,port);
    }
    connect(socket_, SIGNAL(readyRead()), this, SLOT(msgFromServer()));
}

void FileReceiver::creatFile(QString name, qint64 size)
{
    this->file_.setFileName(name);
    this->fileSize_ = size;
    this->receiveSize_ = 0;
    this->fileName_ = name;
    if(!file_.open(QIODevice::WriteOnly))
    {
        qDebug()<<"creat file failed!";
        return;
    }
    qDebug()<<"creat file success!";
}

void FileReceiver::msgFromServer()
{
    QByteArray buf = socket_->readAll();
    if(!file_.exists() ||fileName_.isEmpty() || fileSize_ == 0)
    {
        qDebug()<<"write without file info";
        return;
    }
    qint64 len = file_.write(buf);
    if(len > 0)
        receiveSize_ += len;
    else
        qDebug()<<"file write error";
    if(receiveSize_ == fileSize_)
    {
        qDebug()<<"file write success";
        file_.close();
        socket_->disconnectFromHost();
        socket_->close();
        emit receiveFinished();
    }
}
//--------------------------------------------------------------------------------------------------------

QString getLocalIPv4()
{
    QString hostname = QHostInfo::localHostName();
    QHostInfo hostinfo = QHostInfo::fromName(hostname);
    QString localip;
    QList<QHostAddress> addList = hostinfo.addresses();
    if(!addList.isEmpty())
    {
        for(int i = 0;i < addList.size();i++)
        {
            if(QAbstractSocket::IPv4Protocol == addList.at(i).protocol())
            {
                localip = addList.at(i).toString();
                break;
            }
        }
    }
    return localip;
}
