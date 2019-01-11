// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/TIFFCommon.hpp>

#include <selene/img_io/_impl/TIFFDetail.hpp>

#include <cstdio>
#include <mutex>
#include <sstream>
#include <thread>

namespace sln {

std::ostream& operator<<(std::ostream& os, const TiffImageLayout& info)
{
  using namespace impl::tiff;

  os << "TiffImageLayout: size = (" << info.width << " x " << info.height << "), samples_per_pixel = " << info.samples_per_pixel << ", bits_per_sample = " << info.bits_per_sample << '\n';
  os << "                 planar_config = " << planar_config_to_string(info.planar_config)
     << ", photometric = " << photometric_to_string(info.photometric)
     << ", sample_format = " << sample_format_to_string(info.sample_format)
     << ", compression = " << compression_to_string(info.compression) << '\n';
  return os;
}

namespace {

  std::once_flag tiff_handlers_once_flag;
  std::mutex tiff_message_log_mutex;
  MessageLog tiff_message_log;

  void warning_handler(const char* module, const char* fmt, va_list args)
  {
    const auto thread_id = std::this_thread::get_id();
    std::ostringstream oss;
    oss << thread_id;
    std::string thread_id_str = oss.str();

    char buf[4096];  // Hoping that this is enough...
    int nr_chars_written = sprintf(buf, "LIBTIFF_WARNING: [tid=%s] [%s] ", thread_id_str.c_str(), module);
    vsprintf(buf + nr_chars_written, fmt, args);

    std::lock_guard<std::mutex> lock(tiff_message_log_mutex);
    tiff_message_log.add_message(buf, MessageType::Warning);
  }

  void error_handler(const char *module, const char *fmt, va_list args)
  {
    const auto thread_id = std::this_thread::get_id();
    std::ostringstream oss;
    oss << thread_id;
    std::string thread_id_str = oss.str();

    char buf[4096];  // Hoping that this is enough...
    int nr_chars_written = sprintf(buf, "LIBTIFF_ERROR: [tid=%s] [%s] ", thread_id_str.c_str(), module);
    vsprintf(buf + nr_chars_written, fmt, args);

    std::lock_guard<std::mutex> lock(tiff_message_log_mutex);
    tiff_message_log.add_message(buf, MessageType::Warning);
  }

  void set_handlers_once()
  {
    TIFFSetWarningHandler(warning_handler);
    TIFFSetErrorHandler(error_handler);
  }

}  // namespace _

MessageLog global_tiff_message_log()
{
  std::lock_guard<std::mutex> lock(tiff_message_log_mutex);
  auto log = tiff_message_log;
  return log;
}

void clear_global_tiff_message_log()
{
  std::lock_guard<std::mutex> lock(tiff_message_log_mutex);
  tiff_message_log.clear();
}

namespace impl {

void tiff_set_handlers()
{
  std::call_once(tiff_handlers_once_flag, set_handlers_once);
}

void tiff_assign_message_log(const MessageLog& message_log, MessageLog* output_message_log)
{
  if (!output_message_log)
  {
    return;
  }

  for (const auto& message : message_log.messages())
  {
    output_message_log->add_message(message);
  }
}

}  // namespace impl

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)