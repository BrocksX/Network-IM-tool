#pragma once
#include <sys/time.h>
#include <iostream>
#include <string>

/**
 * 服务器所使用的基本时间戳类
*/
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    ~Timestamp();

    //获取当前时间
    static Timestamp now();
    //日历格式时间去除符号 如197001010800
    std::string toString(bool showMicroseconds = false) const;
    //转换成日历格式时间
    std::string toFormattedString(bool showMicroseconds = false) const;

    int64_t getMicroSecondsSinceEpoch() const;
    time_t getSecondsSinceEpoch() const;

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
    
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.getMicroSecondsSinceEpoch() < rhs.getMicroSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.getMicroSecondsSinceEpoch() == rhs.getMicroSecondsSinceEpoch();
}


inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.getMicroSecondsSinceEpoch() + delta);
}