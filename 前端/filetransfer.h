#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileInfo>
#include <QHostInfo>

class FileSender : public QObject
{
    Q_OBJECT
public:
    explicit FileSender(QObject *parent = nullptr);
    void     listen(const unsigned &port = 0);
    bool     selectFile(QString path);
    bool     sendFile();
    QString  getIP() const;
    quint16  getPort() const;
    QString  getFileName() const;
    qint64   getFileSize() const;
    QTcpServer*  getServerSocket() const;

private:
    QTcpServer *server_;
    QTcpSocket *socket_;
    QString ip_;
    quint16 port_;
    QFile file_;
    QString fileName_;
    qint64  fileSize_;
};

class FileReceiver : public QObject
{
    Q_OBJECT
public:
    explicit FileReceiver(QObject *parent = nullptr);
    void     connectHost(const QString &ip, const quint16 &port);
    void     creatFile(QString name, qint64 size);

private slots:
    void     msgFromServer();

signals:
    void     receiveFinished();

private:
    QTcpSocket *socket_;
    QString ip_;
    quint16 port_;
    QFile file_;
    QString fileName_;
    qint64  fileSize_;
    qint64  receiveSize_;
};

#endif // FILETRANSFER_H
