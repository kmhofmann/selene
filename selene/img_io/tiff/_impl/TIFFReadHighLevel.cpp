// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/tiff/_impl/TIFFReadHighLevel.hpp>

#include <selene/base/MessageLog.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/interop/DynImageToImage.hpp>

#include <selene/img_io/tiff/_impl/TIFFDetail.hpp>

#include <algorithm>
#include <limits>
#include <sstream>
#include <vector>

namespace sln {
namespace impl {

namespace {

std::uint8_t get_r(std::uint32_t x) // ABGR
{
  return static_cast<std::uint8_t>(x & 0x000000ff);
}

std::uint8_t get_g(std::uint32_t x) // ABGR
{
  return static_cast<std::uint8_t>((x >> 8) & 0x000000ff);
}

std::uint8_t get_b(std::uint32_t x) // ABGR
{
  return static_cast<std::uint8_t>((x >> 16) & 0x000000ff);
}

std::uint8_t get_a(std::uint32_t x) // ABGR
{
  return static_cast<std::uint8_t>((x >> 24) & 0x000000ff);
}

}  // namespace _

bool read_data_high_level(TIFF* tif,
                          const sln::TiffImageLayout& src,
                          sln::DynImage& dyn_img,
                          sln::MessageLog& message_log)
{
  const auto width = src.width;
  const auto height = src.height;

  const auto nr_pixels = std::size_t{width * height};
  const auto raster_len = std::size_t{nr_pixels * sizeof (uint32)};
  auto raster = (uint32*)_TIFFmalloc(static_cast<tmsize_t>(raster_len));
  assert (raster != nullptr);

  int res = TIFFReadRGBAImage(tif, width, height, raster, 0);

  if (res == 0)
  {
    message_log.add("Could not read using high-level interface (TIFFReadRGBAImage).", MessageType::Error);
    return false;
  }

  // 'raster' now contains a packed array of ABGR image data

  // allocate data for the whole target image
  const auto nr_bytes_per_channel_out = std::max(std::int16_t{1}, static_cast<std::int16_t>(src.bits_per_sample >> 3));
  impl::tiff::OutputLayout out(to_pixel_length(src.width), to_pixel_length(src.height),
                               static_cast<std::int16_t>(src.samples_per_pixel), nr_bytes_per_channel_out,
                               impl::tiff::photometric_to_pixel_format(src.photometric, src.samples_per_pixel),
                               impl::tiff::sample_format_to_sample_format(src.sample_format));

  const auto dyn_img_layout = UntypedLayout{to_pixel_length(src.width), to_pixel_length(src.height),
                                            std::int16_t{4}, std::int16_t{1}, Stride{0}};
  const auto dyn_img_semantics = UntypedImageSemantics{PixelFormat::RGBA,
                                                       impl::tiff::sample_format_to_sample_format(src.sample_format)};
  dyn_img.reallocate(dyn_img_layout, sln::ImageRowAlignment{0}, dyn_img_semantics);

  // convert each pixel from ABGR to RGBA
  auto dyn_ptr = dyn_img.byte_ptr();
  for (std::size_t i = 0; i < nr_pixels; ++i)
  {
    uint32_t px = raster[i];
    *dyn_ptr++ = get_r(px);
    *dyn_ptr++ = get_g(px);
    *dyn_ptr++ = get_b(px);
    *dyn_ptr++ = get_a(px);
  }

  _TIFFfree(raster);

  return true;
}

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)