#pragma once

#include "Channel.h"
#include "Socket.h"
#include "noncopyable.h"
#include <functional>

class InetAddress;
class EventLoop;

class Acceptor : noncopyable {
  public:
    using NewConnectionCallback =
        std::function<void(int sockfd, const InetAddress &)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) {
        newConnectionCallback_ = std::move(cb);
    }

    bool listenning() const { return listenning_; }
    void listen();

  private:
    // listenfd有事件发生了，就是有新用户连接了
    void handleRead();

    EventLoop *loop_; // Acceptor用的就是用户定义的那个baseloop（mainloop）
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};