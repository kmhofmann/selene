// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_THREAD_DETAIL_CALLABLE_HPP
#define SELENE_THREAD_DETAIL_CALLABLE_HPP

#include <memory>

namespace sln {
namespace detail {

/// \cond INTERNAL

// Inspired by "C++ Concurrency in Action", first edition, listing 9.2
class Callable
{
private:
  template <typename T>
  struct CallableType;

public:
  Callable() = default;
  ~Callable() = default;

  template <typename T>
  explicit Callable(T&& func) : callable_(new CallableType<T>(std::forward<T>(func)))
  {
  }

  Callable(const Callable&) = delete;
  Callable& operator=(const Callable&) = delete;

  Callable(Callable&&) = default;
  Callable& operator=(Callable&&) = default;

  bool valid()
  {
    return callable_ != nullptr;
  }

  void call()
  {
    callable_->call();
  }

private:
  struct CallableBase
  {
    CallableBase() = default;
    virtual ~CallableBase() = default;

    CallableBase(const CallableBase&) = default;
    CallableBase& operator=(const CallableBase&) = default;

    CallableBase(CallableBase&&) = default;
    CallableBase& operator=(CallableBase&&) = default;

    virtual void call() = 0;
  };

  template <typename T>
  struct CallableType : public CallableBase
  {
    T func;

    explicit CallableType(T&& func_) : func(std::move(func_))
    {
    }

    void call() override
    {
      func();
    }
  };

  std::unique_ptr<CallableBase> callable_;
};

/// \endcond

}  // namespace detail
}  // namespace sln

#endif  // SELENE_THREAD_DETAIL_CALLABLE_HPP
