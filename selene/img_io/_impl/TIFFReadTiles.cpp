// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/_impl/TIFFReadTiles.hpp>

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/_impl/Utils.hpp>
#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/typed/Image.hpp>

#include <selene/img_io/_impl/TIFFDetail.hpp>

#include <algorithm>
#include <limits>
#include <iostream>
#include <sstream>
#include <vector>

namespace sln {
namespace impl {

namespace {

bool read_data_tiles_interleaved(TIFF* tif,
                                 const sln::TiffImageLayout& src,
                                 const sln::impl::tiff::ImageLayoutTiles& tile_layout,
                                 const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                                 const sln::impl::tiff::YCbCrConverter& ycbcr_converter,
                                 const sln::impl::tiff::LabConverter& lab_converter,
                                 const sln::impl::tiff::OutputLayout& out,
                                 sln::MutableDynImageView& dyn_img_view,
                                 sln::MessageLog& message_log)
{
  using value_type = PixelIndex::value_type;

  if (src.is_format_ycbcr())
  {
    assert(src.samples_per_pixel == 3);
    ycbcr_info.check_tile_size(src.width, src.height, tile_layout.width, tile_layout.height, message_log);
  }

  constexpr uint16 sample_index = 0;

  // For each tile...
  for (auto src_y = 0_idx; src_y < static_cast<value_type>(src.height); src_y += tile_layout.height)
  {
    for (auto src_x = 0_idx; src_x < static_cast<value_type>(src.width); src_x += tile_layout.width)
    {
      // Read tile data into buffer
      std::vector<std::uint8_t> buf(static_cast<std::size_t>(tile_layout.size_bytes));
      auto nr_bytes_read = TIFFReadTile(tif, buf.data(), static_cast<uint32>(src_x), static_cast<uint32>(src_y), 0, sample_index);
      assert(nr_bytes_read <= static_cast<std::ptrdiff_t>(buf.size()));

      if (nr_bytes_read < 0)
      {
        message_log.add("While reading tile: nr_bytes_read == " + std::to_string(nr_bytes_read), MessageType::Error);
        return false;
      }

      if (src.is_format_ycbcr())
      {
        assert(out.nr_bytes_per_channel == 1);
        buf = convert_ycbcr_to_rgb_interleaved(buf, nr_bytes_read, tile_layout.width, tile_layout.height, ycbcr_info, ycbcr_converter);
        nr_bytes_read = static_cast<tmsize_t>(buf.size());
      }
      else if (src.is_format_lab())
      {
        assert(out.nr_bytes_per_channel == 1);
        buf = convert_lab_to_rgb_interleaved(buf, nr_bytes_read, tile_layout.width, tile_layout.height, lab_converter);
        nr_bytes_read = static_cast<tmsize_t>(buf.size());
      }
      else if (src.is_format_grayscale())
      {
        if (src.bits_per_sample == 1)
        {
          buf = impl::tiff::convert_single_channel_1bit_to_8bit(buf, nr_bytes_read, tile_layout.width, tile_layout.height);
          nr_bytes_read = static_cast<tmsize_t>(buf.size());
        }
        else if (src.bits_per_sample == 4)
        {
          buf = impl::tiff::convert_single_channel_4bit_to_8bit(buf, nr_bytes_read, tile_layout.width, tile_layout.height);
          nr_bytes_read = static_cast<tmsize_t>(buf.size());
        }
      }

      std::uint8_t* data_begin = buf.data();
      std::uint8_t* data_end = buf.data() + nr_bytes_read;

      if (src.inverted())
      {
        std::for_each(data_begin, data_end, [](auto& px){
          px = static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() - px); });
      }

      // Data is stored interleaved.
      const auto dst_x = src_x;
      const auto this_tile_width = std::min(tile_layout.width, src.width - static_cast<std::uint32_t>(src_x));
      const auto this_tile_height = std::min(tile_layout.height, src.height - static_cast<std::uint32_t>(src_y));

      std::size_t i = 0;
      for (PixelIndex dst_y = src_y; dst_y < static_cast<value_type>(static_cast<std::uint32_t>(src_y) + this_tile_height); ++dst_y, ++i)
      {
        const std::size_t nr_bytes_to_write = this_tile_width * to_unsigned(out.nr_channels) * to_unsigned(out.nr_bytes_per_channel);
        const std::size_t nr_bytes_offset = tile_layout.width * to_unsigned(out.nr_channels) * to_unsigned(out.nr_bytes_per_channel);
        const auto img_ptr_y_start = dyn_img_view.byte_ptr(dst_x, dst_y);
        const auto buf_ptr_start = buf.data() + i * nr_bytes_offset;

        auto img_ptr_end = dyn_img_view.byte_ptr() + dyn_img_view.total_bytes();
        const std::size_t max_bytes_to_write = std::min(static_cast<std::size_t>(nr_bytes_to_write),
                                                        static_cast<std::size_t>(img_ptr_end - img_ptr_y_start));

        if (max_bytes_to_write < nr_bytes_to_write)
        {
          message_log.add("Writing fewer bytes than we should...", MessageType::Warning);
        }

        std::memcpy(static_cast<void*>(img_ptr_y_start), buf_ptr_start, max_bytes_to_write);
      }
    }
  }

  return true;
}

bool read_data_tiles_planar(TIFF* tif,
                            const sln::TiffImageLayout& src,
                            const sln::impl::tiff::ImageLayoutTiles& tile_layout,
                            const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                            const sln::impl::tiff::YCbCrConverter& /*ycbcr_converter*/,
                            const sln::impl::tiff::LabConverter& /*lab_converter*/,
                            const sln::impl::tiff::OutputLayout& /*out*/,
                            sln::MutableDynImageView& dyn_img_view,
                            sln::MessageLog& message_log)
{
  using value_type = PixelIndex::value_type;

  message_log.add("Case TILES / PLANAR not implemented.", MessageType::Error);
  return false;

  if (src.is_format_ycbcr())
  {
    assert(src.samples_per_pixel == 3);
    ycbcr_info.check_tile_size(src.width, src.height, tile_layout.width, tile_layout.height, message_log);

    message_log.add("Case TILES / PLANAR / YCBCR not implemented.", MessageType::Error);
    return false;
  }

  if (src.is_format_lab())
  {
    message_log.add("Case TILES / PLANAR / LAB not implemented.", MessageType::Error);
    return false;
  }

  for (uint16 sample_index = 0; sample_index < src.samples_per_pixel; ++sample_index)
  {
    for (auto src_y = 0_px; src_y < static_cast<value_type>(src.height); src_y += tile_layout.height)
    {
      for (auto src_x = 0_px; src_x < static_cast<value_type>(src.width); src_x += tile_layout.width)
      {
        std::vector<std::uint8_t> buf(static_cast<std::size_t>(tile_layout.size_bytes));
        const auto nr_bytes_read = TIFFReadTile(tif, buf.data(), static_cast<uint32>(src_x), static_cast<uint32>(src_y), 0, sample_index);
        assert(nr_bytes_read <= static_cast<std::ptrdiff_t>(buf.size()));

        if (nr_bytes_read < 0)
        {
          message_log.add("ERROR: nr_bytes_read = " + std::to_string(nr_bytes_read), MessageType::Error);
          return false;
        }

        std::uint8_t* data_begin = buf.data();
        std::uint8_t* data_end = buf.data() + nr_bytes_read;

        if (src.inverted())
        {
          std::for_each(data_begin, data_end, [](auto& px){
            px = static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() - px); });
        }

        // TODO: Copy buffer into target image
        // ..
        // ...
        return false;
      }
    }
  }

  return true;
}

} // namespace _


template <typename DynImageOrView>
bool read_data_tiles(TIFF* tif,
                     const sln::TiffImageLayout& src,
                     const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                     const sln::impl::tiff::YCbCrConverter& ycbcr_converter,
                     const sln::impl::tiff::LabConverter& lab_converter,
                     DynImageOrView& dyn_img_or_view,
                     sln::MessageLog& message_log)
{
  const sln::impl::tiff::ImageLayoutTiles tile_layout(
      impl::tiff::get_field<uint32>(tif, TIFFTAG_TILEWIDTH),
      impl::tiff::get_field<uint32>(tif, TIFFTAG_TILELENGTH),
      impl::tiff::get_field<uint32>(tif, TIFFTAG_TILEDEPTH, 1),
      TIFFTileSize(tif));

//  message_log.add(str(oss() << tile_layout), MessageType::Message);

  // --------------- Output to sln::ImageData<>:

  // Allocate data for the whole target image

  // The minimum bit depth is 8 (1 byte/channel)
  const auto nr_bytes_per_channel_out = std::max(std::uint16_t{1}, static_cast<std::uint16_t>(src.bits_per_sample >> 3));
  sln::impl::tiff::OutputLayout out(to_pixel_length(src.width), to_pixel_length(src.height),
                                    to_signed(src.samples_per_pixel), to_signed(nr_bytes_per_channel_out),
                                    impl::tiff::photometric_to_pixel_format(src.photometric, src.samples_per_pixel),
                                    impl::tiff::sample_format_to_sample_format(src.sample_format));
//  message_log.add(str(oss() << out), MessageType::Message);

  const auto pixel_format = [&](){
    if (src.is_format_ycbcr() || src.is_format_lab())
    {
      return PixelFormat::RGB;
    }
    return out.pixel_format;
  }();

  const auto output_layout = UntypedLayout{out.width, out.height, out.nr_channels, out.nr_bytes_per_channel};
  const auto output_semantics = UntypedImageSemantics{pixel_format, out.sample_format};
  bool prepare_success = sln::impl::prepare_image_or_view(dyn_img_or_view, output_layout, output_semantics);

  if (!prepare_success)
  {
    return false;
  }

  if (src.planar_config == TIFFPlanarConfig::Contiguous)
  {
    return read_data_tiles_interleaved(tif, src, tile_layout, ycbcr_info, ycbcr_converter, lab_converter, out, dyn_img_or_view.view(), message_log);
  }
  else  // planar_config == TIFFPlanarConfig::Separate
  {
    return read_data_tiles_planar(tif, src, tile_layout, ycbcr_info, ycbcr_converter, lab_converter, out, dyn_img_or_view.view(), message_log);
  }
}

// Explicit instantiations:
template bool read_data_tiles(TIFF*,
                              const sln::TiffImageLayout&,
                              const sln::impl::tiff::YCbCrInfo&,
                              const sln::impl::tiff::YCbCrConverter&,
                              const sln::impl::tiff::LabConverter&,
                              sln::DynImage&,
                              sln::MessageLog&);
template bool read_data_tiles(TIFF*,
                              const sln::TiffImageLayout&,
                              const sln::impl::tiff::YCbCrInfo&,
                              const sln::impl::tiff::YCbCrConverter&,
                              const sln::impl::tiff::LabConverter&,
                              sln::MutableDynImageView&,
                              sln::MessageLog&);

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)