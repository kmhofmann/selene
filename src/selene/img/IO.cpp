// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/IO.hpp>

namespace sln {

namespace detail {

void add_messages(const MessageLog& messages_src, MessageLog* messages_dst)
{
  if (messages_dst == nullptr)
  {
    return;
  }

  for (const auto& msg : messages_src.messages())
  {
    messages_dst->add_message(msg);
  }
}

}  // namespace detail

}  // namespace sln
