#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include "noncopyable.h"

class ThreadPool
{
public:
    DISALLOW_COPY_AND_MOVE(ThreadPool);
    explicit ThreadPool(int size = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <class F, class... Args>
    auto add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queue_mtx_;
  std::condition_variable cv_;
  std::atomic<bool> stop_{false};
};

//使用右值移动、完美转发阻止拷贝
template <class F, class... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mtx_);
    if (stop_)
    {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    threads_.emplace_back([task]() { (*task)(); });
  }
  cv_.notify_one();
  return res;
}