// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_MESSAGE_LOG_HPP
#define SELENE_BASE_MESSAGE_LOG_HPP

#include <string>
#include <vector>

namespace selene {

/** \class MessageLog
 * Very simple message log, containing a collection of messages.
 *
 * Used inside the library, for example, to return warning or error messages when reading or writing image data.
 */
class MessageLog
{
public:
  using Messages = std::vector<std::string>; ///< A message collection.
  MessageLog() = default;

  const Messages& messages() const;

  // TODO: Replace by std::string_view
  void add_message(const char* message);
  void add_message(const std::string& message);

private:
  std::vector<std::string> messages_;
};

// ---------------
// Implementation:

/** Returns the message collection.
 *
 * \return A reference to the internally managed message log (in form of a std::vector<std::string>&).
 */
inline const MessageLog::Messages& MessageLog::messages() const
{
  return messages_;
}

/** Adds a message to the message log.
 *
 * \param message Message text.
 */
inline void MessageLog::add_message(const char* message)
{
  messages_.emplace_back(std::string(message));
}

/** Adds a message to the message log.
 *
 * \param message Message text.
 */
inline void MessageLog::add_message(const std::string& message)
{
  messages_.push_back(message);
}

} // namespace selene

#endif // SELENE_BASE_MESSAGE_LOG_HPP