#pragma once
#include "noncopyable.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cassert>

class EventLoop;
class Acceptor;
class Connection;
class Socket;
class ThreadPool;
/**
 * TcpServer 是一个可以支持多线程主从Reactor模式的服务器，通过注册回调函数实现业务逻辑
*/
class TcpServer
{
public:
    explicit TcpServer(const char* ip, const uint16_t &port);
    ~TcpServer();

    DISALLOW_COPY_AND_MOVE(TcpServer);

    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
    void setMessageCallback(std::function<void(Connection *)> fn);
    void setConnectionCallback(std::function<void(Connection *)> fn);
    void start();

private:
    std::unique_ptr<EventLoop> mainReactor_;
    std::vector<std::unique_ptr<EventLoop>> sub_reactors_;

    std::unique_ptr<Acceptor> acceptor_;
    std::unordered_map<int, std::unique_ptr<Connection>> connections_;

    std::unique_ptr<ThreadPool>threadPool_;
    std::function<void(Connection *)> connectionCallback_;
    std::function<void(Connection *)> messageCallback_;
};