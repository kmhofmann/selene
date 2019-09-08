// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBPNG)

#include <selene/img_io/png/_impl/Detail.hpp>

#include <stdexcept>

namespace sln::impl {

void error_handler(png_structp png_ptr, const char* msg)
{
  auto& err_man = *static_cast<PNGErrorManager*>(png_get_error_ptr(png_ptr));
  err_man.message_log.add(std::string("Error: ") + std::string(msg), MessageType::Error);
  err_man.error_state = true;
  std::longjmp(png_jmpbuf(png_ptr), 0);
}

void warning_handler(png_structp png_ptr, const char* msg)
{
  auto& err_man = *static_cast<PNGErrorManager*>(png_get_error_ptr(png_ptr));
  err_man.message_log.add(std::string("Warning: ") + std::string(msg), MessageType::Warning);
}

}  // namespace sln::impl

#endif  // defined(SELENE_WITH_LIBPNG)
