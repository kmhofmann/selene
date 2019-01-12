// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img_io/IO.hpp>

namespace sln {

namespace impl {

void add_messages(const MessageLog& message_log_src, MessageLog* message_log_dst)
{
  if (message_log_dst == nullptr)
  {
    return;
  }

  for (const auto& msg : message_log_src.messages())
  {
    message_log_dst->add(msg);
  }
}

}  // namespace impl

}  // namespace sln
