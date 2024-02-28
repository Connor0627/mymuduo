#pragma once

#include "Thread.h"
#include "noncopyable.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

class EventLoop;

class EventLoopThread : noncopyable {
  public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const std::string &name = std::string());
    ~EventLoopThread();
    EventLoop *startLoop();

  private:
    // 下面这个方法是在单独的新线程里面运行的
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};