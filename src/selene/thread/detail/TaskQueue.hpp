#ifndef SELENE_THREAD_DETAIL_TASK_QUEUE_HPP
#define SELENE_THREAD_DETAIL_TASK_QUEUE_HPP

#include <atomic>
#include <condition_variable>
#include <deque>
#include <limits>
#include <mutex>

#include <selene/thread/detail/Callable.hpp>

namespace selene {
namespace thread {

namespace detail {

/// \cond INTERNAL

// Inspired by Sean Parent's talk "Better Code: Concurrency"
// (https://www.slideshare.net/sermp/better-code-concurrency)
class TaskQueue
{
public:
  TaskQueue();
  ~TaskQueue() = default;

  TaskQueue(const TaskQueue&) = delete;
  TaskQueue& operator=(const TaskQueue& other) = delete;

  TaskQueue(TaskQueue&& other) noexcept;
  TaskQueue& operator=(TaskQueue&& other) noexcept;

  template <typename Func> bool try_push(Func&& task);
  template <typename Func> void push(Func&& task);

  bool try_pop(Callable& task);
  bool pop(Callable& task);

  void set_finished();

private:
  std::deque<Callable> tasks_;
  mutable std::mutex mutex_;
  std::condition_variable cond_;
  std::atomic<bool> finished_;
};

inline TaskQueue::TaskQueue()
  : finished_(false)
{
}


inline TaskQueue::TaskQueue(TaskQueue&& other) noexcept
{
  std::lock_guard<std::mutex> lock(other.mutex_);

  tasks_ = std::move(other.tasks_);
  finished_.store(other.finished_);
}


inline TaskQueue& TaskQueue::operator=(TaskQueue&& other) noexcept
{
  if (&other == this)
  {
    return *this;
  }

  std::lock(mutex_, other.mutex_);
  std::lock_guard<std::mutex> lock0(mutex_, std::adopt_lock);
  std::lock_guard<std::mutex> lock1(other.mutex_, std::adopt_lock);

  tasks_ = std::move(other.tasks_);
  finished_.store(other.finished_);

  return *this;
}


template <typename Func> bool TaskQueue::try_push(Func&& task)
{
  {
    std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);

    if (!lock)
    {
      return false;
    }

    tasks_.emplace_back(std::forward<Func>(task));
  }

  cond_.notify_one();
  return true;
}


template <typename Func> void TaskQueue::push(Func&& task)
{
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.emplace_back(std::forward<Func>(task));
  }

  cond_.notify_one();
}


inline bool TaskQueue::try_pop(Callable& task)
{
  std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);

  if (!lock || tasks_.empty())
  {
    return false;
  }

  task = std::move(tasks_.front());
  tasks_.pop_front();
  return true;
}


inline bool TaskQueue::pop(Callable& task)
{
  std::unique_lock<std::mutex> lock(mutex_);

  while (tasks_.empty() && !finished_)
  {
    cond_.wait(lock);
  }

  if (tasks_.empty() && finished_)
  {
    return false;
  }

  task = std::move(tasks_.front());
  tasks_.pop_front();
  return true;
}


inline void TaskQueue::set_finished()
{
  finished_ = true;
  cond_.notify_all();
}

/// \endcond

} // namespace detail

} // namespace thread
} // namespace selene

#endif // SELENE_THREAD_DETAIL_TASK_QUEUE_HPP
