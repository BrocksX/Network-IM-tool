#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Redis.h"
#include "noncopyable.h"

class RedisConnectPool
{
public:
    DISALLOW_COPY_AND_MOVE(RedisConnectPool);
    RedisConnectPool(const std::string ip, uint16_t port, int size, const std::string passwd = "");
    ~RedisConnectPool();

    bool connect();
    Redis* getConnect();
    void releaseConnect(Redis *conn);

private:
    int size_;
    std::string ip_;
    std::string password_;
    uint16_t port_;
    std::queue<Redis *> connectPool_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
