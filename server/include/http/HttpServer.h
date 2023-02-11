#pragma once

#include "TcpServer.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include "Timestamp.h"
#include "Connection.h"
#include <string>

class HttpRequest;
class HttpResponse;
class InetAddress;
class HttpServer
{
public:
    DISALLOW_COPY_AND_MOVE(HttpServer);
    using HttpCallback = std::function<void (const HttpRequest&, HttpResponse*)>;

    HttpServer(const char* ip, const uint16_t &port);

    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }
    
    void start();

private:
    void onConnection(Connection* conn);
    void onMessage(Connection* conn);
    void onRequest(Connection* , const HttpRequest&);

    TcpServer server_;
    HttpCallback httpCallback_;
};
