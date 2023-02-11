#pragma once
#include "noncopyable.h"
#include <functional>
#include <string>
#include <memory>

class EventLoop;
class Socket;
class Channel;
class Buffer;
/**
 * Acceptor类的负责新建连接，Connection类负责处理事件的逻辑。
 * 通过绑定回调函数实现业务逻辑
*/
class Connection
{
public:
    enum State
    {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
    Connection(EventLoop *loop, Socket *socket);
    ~Connection();
    DISALLOW_COPY_AND_MOVE(Connection);

    //读写操作
    void read();
    void write();
    void send(const std::string &msg);
    void send(const Buffer &buffer);

    void setDeleteConnectionCallback(std::function<void(Socket *)> const &callback);
    void setMessageCallback(std::function<void(Connection *)> const &callback);

    State getState();
    void close();
    void setSendBuffer(const char *str);
    Buffer *getReadBuffer();
    const char *readBuffer();
    Buffer *getSendBuffer();
    const char *sendBuffer();
    void getlineSendBuffer();
    Socket *getSocket();


private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    State state_;
    std::unique_ptr<Buffer> readBuffer_;
    std::unique_ptr<Buffer> sendBuffer_;
    std::function<void(Socket *)> deleteConnectioinCallback_;
    std::function<void(Connection *)> messageCallback_;

    void readNonBlocking();
    void writeNonBlocking();
    void readBlocking();
    void writeBlocking();
};