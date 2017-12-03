// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DETAIL_UTIL_HPP
#define SELENE_IMG_DETAIL_UTIL_HPP

#include <selene/base/MessageLog.hpp>

namespace selene {
namespace img {
namespace detail
{

template <typename T> inline void assign_message_log(const T& obj, MessageLog* messages)
{
  if (messages)
  {
    *messages = obj.message_log();
  }
}

} // namespace detail
} // namespace img
} // namespace selene

#endif // SELENE_IMG_DETAIL_UTIL_HPP
