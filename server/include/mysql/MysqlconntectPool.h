#pragma once

#include "Mysql.h"
#include "noncopyable.h"
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

class MysqlConnectionPool
{
public:
    DISALLOW_COPY_AND_MOVE(MysqlConnectionPool);

    MysqlConnectionPool(const std::string &ip, const uint16_t &port, const std::string &username, const std::string &password, 
        const std::string &dbName, const int &size, const int &timeout, const int &maxIdletime);
    ~MysqlConnectionPool();
    std::shared_ptr<Mysql> getConnection();

private:
    void addConnection();

    std::string ip_;
    std::string user_;
    std::string passwd_;
    std::string dbName_;
    unsigned short port_;
    int size_;
    int currentSize_;
    int timeout_;
    int maxIdleTime_;
    std::queue<Mysql*> connectPool_; // 连接池队列
    std::mutex mutex_; 
    std::condition_variable cv_;
};