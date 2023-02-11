#pragma once
#include "noncopyable.h"
#include <functional>
#include "Timestamp.h"

class Timer
{
private:
    std::function<void()> callback_;
    Timestamp expiration_;
    double interval_;
    bool repeat_;

public:
    Timer(std::function<void()> cb, Timestamp time, double interval);
    ~Timer() = default;

    void run() const;
    Timestamp getExpiration() const;
    bool isRepeat() const;
    void restart(Timestamp now);
};

