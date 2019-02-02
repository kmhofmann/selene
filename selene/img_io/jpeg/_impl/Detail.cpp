// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBJPEG)

#include <selene/img_io/jpeg/_impl/Detail.hpp>

#include <stdexcept>

namespace sln {
namespace impl {

J_COLOR_SPACE color_space_pub_to_lib(JPEGColorSpace color_space)
{
  switch (color_space)
  {
    case JPEGColorSpace::Unknown: return JCS_UNKNOWN;
    case JPEGColorSpace::Grayscale: return JCS_GRAYSCALE;
    case JPEGColorSpace::RGB: return JCS_RGB;
    case JPEGColorSpace::YCbCr: return JCS_YCbCr;
    case JPEGColorSpace::CMYK: return JCS_CMYK;
    case JPEGColorSpace::YCCK: return JCS_YCCK;
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    case JPEGColorSpace::EXT_RGB: return JCS_EXT_RGB;
    case JPEGColorSpace::EXT_BGR: return JCS_EXT_BGR;
    case JPEGColorSpace::EXT_RGBA: return JCS_EXT_RGBA;
    case JPEGColorSpace::EXT_BGRA: return JCS_EXT_BGRA;
    case JPEGColorSpace::EXT_ABGR: return JCS_EXT_ABGR;
    case JPEGColorSpace::EXT_ARGB: return JCS_EXT_ARGB;
    case JPEGColorSpace::EXT_RGBX: return JCS_EXT_RGBX;
    case JPEGColorSpace::EXT_BGRX: return JCS_EXT_BGRX;
    case JPEGColorSpace::EXT_XBGR: return JCS_EXT_XBGR;
    case JPEGColorSpace::EXT_XRGB: return JCS_EXT_XRGB;
#endif
    default: return JCS_UNKNOWN;
  }
}

JPEGColorSpace color_space_lib_to_pub(J_COLOR_SPACE color_space)
{
  switch (color_space)
  {
    case JCS_UNKNOWN: return JPEGColorSpace::Unknown;
    case JCS_GRAYSCALE: return JPEGColorSpace::Grayscale;
    case JCS_RGB: return JPEGColorSpace::RGB;
    case JCS_YCbCr: return JPEGColorSpace::YCbCr;
    case JCS_CMYK: return JPEGColorSpace::CMYK;
    case JCS_YCCK: return JPEGColorSpace::YCCK;
#if defined(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    case JCS_EXT_RGB: return JPEGColorSpace::EXT_RGB;
    case JCS_EXT_BGR: return JPEGColorSpace::EXT_BGR;
    case JCS_EXT_RGBA: return JPEGColorSpace::EXT_RGBA;
    case JCS_EXT_BGRA: return JPEGColorSpace::EXT_BGRA;
    case JCS_EXT_ABGR: return JPEGColorSpace::EXT_ABGR;
    case JCS_EXT_ARGB: return JPEGColorSpace::EXT_ARGB;
    case JCS_EXT_RGBX: return JPEGColorSpace::EXT_RGBX;
    case JCS_EXT_BGRX: return JPEGColorSpace::EXT_BGRX;
    case JCS_EXT_XBGR: return JPEGColorSpace::EXT_XBGR;
    case JCS_EXT_XRGB: return JPEGColorSpace::EXT_XRGB;
#endif
    default: return JPEGColorSpace::Unknown;
  }
}

void error_exit(j_common_ptr cinfo)
{
  auto& err_man = *reinterpret_cast<JPEGErrorManager*>(cinfo->err);
  (*cinfo->err->output_message)(cinfo);
  err_man.error_state = true;
  std::longjmp(err_man.setjmp_buffer, 1);
}

void output_message(j_common_ptr cinfo)
{
  auto& err_man = *reinterpret_cast<JPEGErrorManager*>(cinfo->err);
  char buffer[JMSG_LENGTH_MAX];
  (*cinfo->err->format_message)(cinfo, buffer);
  err_man.message_log.add(std::string("Error: ") + std::string(buffer), MessageType::Error);
}

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBJPEG)
