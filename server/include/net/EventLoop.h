#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "noncopyable.h"
#include "Timestamp.h"
#include "TimerQueue.h"
#include "Timer.h"

class Poller;
class Channel;
class Timestamp;
class Timer;
class TimerQueue;
/**
 * EventLoop是事件循环类，将需要监听的事务添加到这个事件循环内，每次有事件发生时就会通知，然后根据不同的描述符、事件类型进行处理。
*/
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel *) const;
    void deleteChannel(Channel *) const;

    //定时器相关功能
    Timer* runAt(Timestamp timestamp, std::function<void()>&& cb);
    Timer* runAfter(double waitTime, std::function<void()>&& cb);
    Timer* runEvery(double interval, std::function<void()>&& cb);
    void cancel(Timer* timer);

private:
    std::unique_ptr<Poller> poller_;
    std::vector<Channel*> activeChannels_;
    std::unique_ptr<TimerQueue> timerQueue_;
    bool quit;

};