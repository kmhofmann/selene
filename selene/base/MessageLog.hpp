// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_MESSAGE_LOG_HPP
#define SELENE_BASE_MESSAGE_LOG_HPP

/// @file

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

namespace sln {

enum class MessageType
{
  Headline,
  Verbose,
  Success,
  Message,
  Warning,
  Error
};

/** \brief Very simple message log, containing a collection of messages.
 *
 * Used inside the library, for example, to return warning or error messages when reading or writing image data.
 */
class MessageLog
{
public:
  struct Message
  {
    std::string text;
    MessageType type;
  };

  using Messages = std::vector<Message>;  ///< A message collection.
  MessageLog() = default;

  const Messages& messages() const;

  // TODO: Replace by std::string_view
  void add(const char* text, MessageType type);
  void add(const std::string& text, MessageType type);
  void add(const Message& message);
  void add(Message&& message);

  bool contains_warnings();
  bool contains_errors();
  bool contains_warnings_or_errors();

  void clear();

private:
  std::vector<Message> messages_;
};

std::string message_type_to_string(MessageType type);

std::ostream& operator<<(std::ostream& os, const MessageLog& message_log);

// ---------------
// Implementation:

/** \brief Returns the message collection.
 *
 * \return A reference to the internally managed message log (in form of a std::vector<std::pair<std::string, MessageType>>&).
 */
inline const MessageLog::Messages& MessageLog::messages() const
{
  return messages_;
}

/** \brief Adds a message to the message log.
 *
 * \param text Message text.
 * \param type Message type.
 */
inline void MessageLog::add(const char* text, MessageType type)
{
  messages_.push_back({std::string(text), type});
}

/** \brief Adds a message to the message log.
 *
 * \param text Message text.
 * \param type Message type.
 */
inline void MessageLog::add(const std::string& text, MessageType type)
{
  messages_.push_back({text, type});
}

/** \brief Adds a message to the message log.
 *
 * \param message Message, consisting of text and type.
 */
inline void MessageLog::add(const Message& message)
{
  messages_.push_back(message);
}

/** \brief Adds a message to the message log.
 *
 * \param message Message, consisting of text and type.
 */
inline void MessageLog::add(Message&& message)
{
  messages_.push_back(std::move(message));
}

/** \brief Clears the message log.
 */
inline void MessageLog::clear()
{
  messages_.clear();
}

}  // namespace sln

#endif  // SELENE_BASE_MESSAGE_LOG_HPP
