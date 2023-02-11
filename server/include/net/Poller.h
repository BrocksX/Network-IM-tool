#pragma once
#include <sys/epoll.h>
#include <vector>
#include "noncopyable.h"

class Channel;
/**
 * 多路事件分发器的核心IO复用模块
*/
class Poller
{
public:
    DISALLOW_COPY_AND_MOVE(Poller);
    Poller();
    ~Poller();

    void updateChannel(Channel*);
    void deleteChannel(Channel*);
    std::vector<Channel*> poll(int timeout = -1);

private:
    int epfd_;
    epoll_event *events_;
};
