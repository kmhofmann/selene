// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_JPEG_DETAIL_HPP
#define SELENE_IMG_IMPL_JPEG_DETAIL_HPP

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/base/MessageLog.hpp>

#include <selene/img_io/jpeg/_impl/Common.hpp>

#include <jpeglib.h>

#include <csetjmp>
#include <cstdio>

namespace sln {
namespace impl {

/// \cond INTERNAL

J_COLOR_SPACE color_space_pub_to_lib(JPEGColorSpace color_space);
JPEGColorSpace color_space_lib_to_pub(J_COLOR_SPACE color_space);

// Error handling structures

struct JPEGErrorManager
{
  jpeg_error_mgr pub;
  std::jmp_buf setjmp_buffer;

  bool error_state = false;
  MessageLog message_log;
};

void error_exit(j_common_ptr cinfo);
void output_message(j_common_ptr cinfo);

/// \endcond

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)

#endif  // SELENE_IMG_IMPL_JPEG_DETAIL_HPP
