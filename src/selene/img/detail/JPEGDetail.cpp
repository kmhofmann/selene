// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG) || defined(SELENE_WITH_LIBJPEG_TURBO)

#include <selene/img/detail/JPEGDetail.hpp>

#include <stdexcept>

namespace selene {
namespace img {
namespace detail {

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
#if defined(SELENE_WITH_LIBJPEG_TURBO)
    case JPEGColorSpace::BGR: return JCS_EXT_BGR;
    case JPEGColorSpace::RGBA: return JCS_EXT_RGBA;
    case JPEGColorSpace::BGRA: return JCS_EXT_BGRA;
    case JPEGColorSpace::ABGR: return JCS_EXT_ABGR;
    case JPEGColorSpace::ARGB: return JCS_EXT_ARGB;
#endif
    default: throw std::runtime_error("Unknown color space");
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
#if defined(SELENE_WITH_LIBJPEG_TURBO)
    case JCS_EXT_BGR: return JPEGColorSpace::BGR;
    case JCS_EXT_RGBA: return JPEGColorSpace::RGBA;
    case JCS_EXT_BGRA: return JPEGColorSpace::BGRA;
    case JCS_EXT_ABGR: return JPEGColorSpace::ABGR;
    case JCS_EXT_ARGB: return JPEGColorSpace::ARGB;
#endif
    default: throw std::runtime_error("Unknown color space");
  }
}

void error_exit(j_common_ptr cinfo)
{
  auto& err_man = *reinterpret_cast<JPEGErrorManager*>(cinfo->err);
  (* cinfo->err->output_message)(cinfo);
  err_man.error_state = true;
  std::longjmp(err_man.setjmp_buffer, 1);
}

void output_message(j_common_ptr cinfo)
{
  auto& err_man = *reinterpret_cast<JPEGErrorManager*>(cinfo->err);
  char buffer[JMSG_LENGTH_MAX];
  (* cinfo->err->format_message)(cinfo, buffer);
  err_man.message_log.add_message(std::string("Error: ") + std::string(buffer));
}

} // namespace detail
} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBJPEG) || defined(SELENE_WITH_LIBJPEG_TURBO)
