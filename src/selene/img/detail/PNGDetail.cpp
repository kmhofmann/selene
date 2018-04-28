// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <selene/img/detail/PNGDetail.hpp>

#include <stdexcept>

namespace sln {
namespace detail {

void error_handler(png_structp png_ptr, const char* msg)
{
  auto& err_man = *static_cast<PNGErrorManager*>(png_get_error_ptr(png_ptr));
  err_man.message_log.add_message(std::string("Error: ") + std::string(msg));
  err_man.error_state = true;
  std::longjmp(png_jmpbuf(png_ptr), 0);
}

void warning_handler(png_structp png_ptr, const char* msg)
{
  auto& err_man = *static_cast<PNGErrorManager*>(png_get_error_ptr(png_ptr));
  err_man.message_log.add_message(std::string("Warning: ") + std::string(msg));
}

}  // namespace detail
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBPNG)