#pragma once
#include <arpa/inet.h>
#include "noncopyable.h"
#include "InetAddress.h"

class InetAddress;
/**
 * 封装socket fd
*/
class Socket
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    DISALLOW_COPY_AND_MOVE(Socket);

    void bind(InetAddress *addr);
    void listen();
    int accept(InetAddress *addr);

    void connect(InetAddress *addr);
    void connect(const char *ip, const uint16_t &port);

    void setNonBlocking();
    bool isNonBlocking();
    int getFd();

private:
    int fd_{-1};
};