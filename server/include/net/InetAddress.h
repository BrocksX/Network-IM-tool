#include <arpa/inet.h>
#include <cstring>
#include "noncopyable.h"
/**
 * 提供了一些地址转换的函数，储存ip地址端口
*/
class InetAddress
{
public:
    DISALLOW_COPY_AND_MOVE(InetAddress);

    InetAddress() = default;
    InetAddress(const char *ip, const uint16_t &port);
    ~InetAddress() = default;

    void setAddr(const sockaddr_in &addr);
    sockaddr_in getAddr();
    const char *getIp();
    uint16_t getPort();

private:
    sockaddr_in addr_{};
};