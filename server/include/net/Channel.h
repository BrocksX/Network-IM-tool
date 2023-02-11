#pragma once
#include "noncopyable.h"
#include <functional>
#include <cstdint>

class EventLoop;
/**
 * 一个Channel类负责一个文件描述符，对不同的服务、不同的事件类型，都可以在Channel中进行不同的处理
*/
class Channel
{
public:
    DISALLOW_COPY_AND_MOVE(Channel);
    Channel(EventLoop *loop, int fd);
    ~Channel();

    //处理时间
    void handleEvent();
    //启动
    void enableRead();

    int getFd();
    uint32_t getListenEvents();
    uint32_t getReadyEvents();
    bool getInEpoll();
    void setInEpoll(bool in = true);
    void useET();

    void setReadyEvents(const uint32_t &ev);
    void setReadCallback(std::function<void()> const &callback);
    void setWriteCallback(std::function<void()> const &callback);

private:
    EventLoop *loop_;
    int fd_;
    uint32_t listenEvents_;
    uint32_t readyEvents_;
    bool inEpoll_;
    std::function<void()> readCallback_;
    std::function<void()> writeCallback_;
};