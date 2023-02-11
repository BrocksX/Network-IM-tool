#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <vector>
#include <string.h>
#include <memory>
#include "noncopyable.h"
#include <unordered_map>

class Redis
{
public:
    DISALLOW_COPY_AND_MOVE(Redis);
    Redis() = default;
    ~Redis() = default;

    bool connect(const std::string &ip, const uint16_t &port, const std::string &password = "");
    void disconnect();

    bool set(const std::string &key, const std::string &value);
    bool setWithTimeout(const std::string &key, const std::string &value, const int64_t &sceonds = 100);
    std::string get(std::string key);

    int64_t hset(const std::string &key, const std::unordered_map<std::string, std::string> &fields_vals);
    bool hget(const std::string &key, const std::string &field, std::string &value);
    bool hmget(const std::string &key, const std::vector<std::string> &fields, std::vector<std::string> &values);
    bool hexists(const std::string &key, const std::string &field);

    void zadd(std::string key, std::string score, std::string member);
    bool zrange(const std::string &key, int64_t start, int64_t stop, std::vector<std::string> &values, bool withScores);

private:
    std::unique_ptr<redisContext> connect_;
    bool execReplyString(const std::vector<std::string> &args, std::string &ret);
    bool execReplyArray(const std::string &cmd, std::vector<std::string> &ret);
    bool execReplyInt(const std::vector<std::string> &args, int64_t &ret);
    bool execReplyStatus(const std::vector<std::string> &args, std::string &ret);
};
