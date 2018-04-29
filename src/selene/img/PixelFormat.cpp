// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/PixelFormat.hpp>

namespace sln {

/** \brief Converts a pixel format value into a character stream representation, e.g. for printing.
 *
 * @param os An output stream.
 * @param pixel_format The pixel format value to be converted.
 * @return A reference to the provided output stream.
 */
std::ostream& operator<<(std::ostream& os, PixelFormat pixel_format)
{
  switch (pixel_format)
  {
    case PixelFormat::Y: os << "PixelFormat::Y"; break;
    case PixelFormat::X: os << "PixelFormat::X"; break;

    case PixelFormat::YA: os << "PixelFormat::YA"; break;
    case PixelFormat::XX: os << "PixelFormat::XX"; break;

    case PixelFormat::RGB: os << "PixelFormat::RGB"; break;
    case PixelFormat::BGR: os << "PixelFormat::BGR"; break;
    case PixelFormat::YCbCr: os << "PixelFormat::YCbCr"; break;
    case PixelFormat::CIELab: os << "PixelFormat::CIELab"; break;
    case PixelFormat::ICCLab: os << "PixelFormat::ICCLab"; break;
    case PixelFormat::XXX: os << "PixelFormat::XXX"; break;

    case PixelFormat::RGBA: os << "PixelFormat::RGBA"; break;
    case PixelFormat::BGRA: os << "PixelFormat::BGRA"; break;
    case PixelFormat::ARGB: os << "PixelFormat::ARGB"; break;
    case PixelFormat::ABGR: os << "PixelFormat::ABGR"; break;
    case PixelFormat::CMYK: os << "PixelFormat::CMYK"; break;
    case PixelFormat::YCCK: os << "PixelFormat::YCCK"; break;
    case PixelFormat::XXXX: os << "PixelFormat::XXXX"; break;

    case PixelFormat::Unknown: os << "PixelFormat::Unknown"; break;
  }

  return os;
}

/** \brief Converts a sample format value into a character stream representation, e.g. for printing.
 *
 * @param os An output stream.
 * @param sample_format The sample type value to be converted.
 * @return A reference to the provided output stream.
 */
std::ostream& operator<<(std::ostream& os, SampleFormat sample_format)
{
  switch (sample_format)
  {
    case SampleFormat::UnsignedInteger: os << "SampleFormat::UnsignedInteger"; break;
    case SampleFormat::SignedInteger: os << "SampleFormat::SignedInteger"; break;
    case SampleFormat::FloatingPoint: os << "SampleFormat::FloatingPoint"; break;
    case SampleFormat::Unknown: os << "SampleFormat::Unknown"; break;
  }

  return os;
}

}  // namespace sln
