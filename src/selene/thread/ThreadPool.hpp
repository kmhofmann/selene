// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_THREAD_THREAD_POOL_HPP
#define SELENE_THREAD_THREAD_POOL_HPP

/// @file

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <selene/thread/detail/TaskQueue.hpp>

namespace selene {
namespace thread {

// Implementation inspired by:
// https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
// https://github.com/stlab/libraries/blob/master/stlab/concurrency/default_executor.hpp
// and
// "C++ Concurrency in Action", first edition, listing 9.2

class ThreadPool
{
public:
  explicit ThreadPool(std::size_t num_threads);
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  template <typename Func, typename... Args>
  auto push(Func&& func, Args&& ... args) -> std::future<typename std::result_of<Func(Args...)>::type>;

  bool empty() const;
  std::size_t size() const;

private:
  std::vector<detail::TaskQueue> queues_;
  std::vector<std::thread> threads_;
  std::atomic<std::size_t> index_;
  std::atomic<std::size_t> num_threads_;
  mutable std::mutex mutex_;

  void run_loop(std::size_t thread_index);
};


template <typename Func, typename... Args>
auto async(ThreadPool& thread_pool, Func&& f, Args&&... args)
{
  return thread_pool.push(std::forward<Func>(f), std::forward<Args>(args)...);
}


inline ThreadPool::ThreadPool(std::size_t num_threads)
    : index_(0), num_threads_(0)
{
  std::lock_guard<std::mutex> lock(mutex_);

  assert(num_threads > 0);
  queues_.resize(num_threads);
  threads_.reserve(num_threads);

  for (std::size_t i = 0; i < num_threads; ++i)
  {
    threads_.emplace_back([&, i] { run_loop(i); });
  }

  num_threads_ = threads_.size();
}


inline ThreadPool::~ThreadPool()
{
  std::lock_guard<std::mutex> lock(mutex_);

  std::for_each(queues_.begin(), queues_.end(), [](detail::TaskQueue& q) { q.set_finished(); });
  std::for_each(threads_.begin(), threads_.end(), [](std::thread& t) { t.join(); });
}


template <typename Func, typename... Args>
auto ThreadPool::push(Func&& func, Args&& ... args) -> std::future<typename std::result_of<Func(Args...)>::type>
{
  using ReturnType = typename std::result_of<Func(Args...)>::type;

  auto f = [func=std::decay_t<Func>(func), args...]() mutable { return func(args...); };
  std::packaged_task<ReturnType()> task(std::move(f));
  std::future<ReturnType> future(task.get_future());

  const auto i = index_++;

  for (std::size_t n = 0; n < num_threads_; ++n)
  {
    if (queues_[(i + n) % num_threads_].try_push(std::move(task)))
    {
      return future;
    }
  }

  queues_[i % num_threads_].push(std::move(task));
  return future;
}


inline bool ThreadPool::empty() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return threads_.empty();
}


inline std::size_t ThreadPool::size() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return threads_.size();
}


inline void ThreadPool::run_loop(std::size_t thread_index)
{
  const auto i = thread_index;

  while (true)
  {
    detail::Callable current_task;

    for (std::size_t n = 0; n < 32 * num_threads_; ++n)
    {
      if (queues_[(i + n) % num_threads_].try_pop(current_task))
      {
        break;
      }
    }

    if (!current_task.valid() && !queues_[i].pop(current_task))
    {
      break;
    }

    current_task.call();
  }
}

} // namespace thread
} // namespace selene

#endif // SELENE_THREAD_THREAD_POOL_HPP