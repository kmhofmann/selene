// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DETAIL_PNG_DETAIL_HPP
#define SELENE_IMG_DETAIL_PNG_DETAIL_HPP

#include <selene/base/MessageLog.hpp>

#include <png.h>

#include <csetjmp>
#include <cstdio>

namespace selene {
namespace img {
namespace detail {

/// \cond INTERNAL

struct PNGErrorManager
{
  bool error_state = false;
  MessageLog message_log;
};

void error_handler(png_structp png_ptr, const char* msg);
void warning_handler(png_structp png_ptr, const char* msg);

/// \endcond

} // namespace detail
} // namespace img
} // namespace selene

#endif // SELENE_IMG_DETAIL_PNG_DETAIL_HPP