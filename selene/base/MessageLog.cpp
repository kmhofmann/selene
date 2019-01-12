// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/MessageLog.hpp>

#include <algorithm>
#include <ostream>

namespace sln {

bool MessageLog::contains_warnings()
{
  return std::any_of(messages_.cbegin(), messages_.cend(),
                     [](const auto& msg) { return msg.type == MessageType::Warning; });
}

bool MessageLog::contains_errors()
{
  return std::any_of(messages_.cbegin(), messages_.cend(),
                     [](const auto& msg) { return msg.type == MessageType::Error; });
}

bool MessageLog::contains_warnings_or_errors()
{
  return std::any_of(messages_.cbegin(), messages_.cend(),
                     [](const auto& msg) { return msg.type == MessageType::Warning
                                               || msg.type == MessageType::Error; });
}

std::string message_type_to_string(MessageType type)
{
  switch (type)
  {
    case MessageType::Headline: return "HEADLINE";
    case MessageType::Verbose:  return "VERBOSE";
    case MessageType::Success:  return "SUCCESS";
    case MessageType::Message:  return "MESSAGE";
    case MessageType::Warning:  return "WARNING";
    case MessageType::Error:    return "ERROR";
    default: return "";
  }
}

std::ostream& operator<<(std::ostream& os, const MessageLog& message_log)
{
  for (const auto& msg : message_log.messages())
  {
    os << '[' << message_type_to_string(msg.type) << "] " << msg.text << '\n';
  }

  return os;
}

}  // namespace sln