#pragma once

#include "Poller.h"

#include <sys/epoll.h>
#include <vector>

class EPollPoller : public Poller {
  public:
    EPollPoller(EventLoop *Loop);
    ~EPollPoller() override;

    // 重写基类Poller的抽象方法
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override; // 从poller中删除channel

  private:
    static const int kInitEventListSize = 16;

    // 填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel通道
    void update(int operation, Channel *channel);
    using EventList = std::vector<epoll_event>;
    int epollfd_;
    EventList events_;
};