// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/tiff/_impl/TIFFReadStrips.hpp>

#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/_impl/Utils.hpp>
#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/typed/Image.hpp>

#include <selene/img_io/tiff/_impl/TIFFDetail.hpp>

#include <algorithm>
#include <limits>
#include <sstream>
#include <vector>

namespace sln {
namespace impl {

namespace {

bool read_data_strips_interleaved(TIFF* tif,
                                  const sln::TiffImageLayout& src,
                                  const sln::impl::tiff::ImageLayoutStrips& strip_layout,
                                  const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                                  const sln::impl::tiff::YCbCrConverter& ycbcr_converter,
                                  const sln::impl::tiff::LabConverter& lab_converter,
                                  const sln::impl::tiff::OutputLayout& out,
                                  sln::MutableDynImageView& dyn_img_view,
                                  sln::MessageLog& message_log)
{
  // For each strip...
  for (tstrip_t strip_index = 0; strip_index < strip_layout.nr_strips; ++strip_index)
  {
    // Read strip data into buffer
    std::vector<std::uint8_t> buf(static_cast<std::size_t>(strip_layout.size_bytes));  // cannot hoist; overwritten below
    auto nr_bytes_read = TIFFReadEncodedStrip(tif, strip_index, buf.data(), -1);
    SELENE_ASSERT(nr_bytes_read <= static_cast<tmsize_t>(buf.size()));

    if (strip_index != strip_layout.nr_strips - 1)
    {
      if (nr_bytes_read >= 0 && nr_bytes_read != static_cast<tmsize_t>(buf.size()))
      {
        message_log.add(
            "Strip " + std::to_string(strip_index) + ": nr_bytes_read (" + std::to_string(nr_bytes_read) +
            ") != buf.size() (" + std::to_string(buf.size()) + ")", MessageType::Warning);
      }
    }

    if (nr_bytes_read < 0)
    {
      message_log.add(
          "Strip " + std::to_string(strip_index) + ": nr_bytes_read == " + std::to_string(nr_bytes_read),
          MessageType::Error);
      return false;
    }

    const auto expected_nr_bytes = [&src, &strip_layout, &ycbcr_info, &out]() {
      if (src.is_format_ycbcr())
      {
        SELENE_ASSERT(out.nr_bytes_per_channel == 1);
        const auto nr_bytes_for_1_channel = strip_layout.rows_per_strip * to_unsigned(out.width) * to_unsigned(out.nr_bytes_per_channel);
        const auto subsample_factor = to_unsigned(ycbcr_info.subsampling_horz * ycbcr_info.subsampling_vert);
        return nr_bytes_for_1_channel + (2 * nr_bytes_for_1_channel / subsample_factor);
      }

      return (strip_layout.rows_per_strip * to_unsigned(out.width) * to_unsigned(out.nr_channels) * src.bits_per_sample) >> 3;
    }();

    if (nr_bytes_read != static_cast<tmsize_t>(expected_nr_bytes)
        && strip_index != static_cast<tstrip_t>(strip_layout.nr_strips - 1))
    {
      message_log.add(
          "nr_bytes_read (" + std::to_string(nr_bytes_read) + ") != expected_nr_bytes (" +
          std::to_string(expected_nr_bytes) + ")", MessageType::Warning);
    }

    const auto rows_in_this_strip = static_cast<uint32>(strip_layout.rows_per_strip * nr_bytes_read / expected_nr_bytes);

    // Modify the buffer, if necessary

    if (src.is_format_ycbcr())
    {
      SELENE_ASSERT(out.nr_bytes_per_channel == 1);
      buf = convert_ycbcr_to_rgb_interleaved(buf, nr_bytes_read, src.width, rows_in_this_strip, ycbcr_info, ycbcr_converter);
      nr_bytes_read = static_cast<tmsize_t>(buf.size());
    }
    else if (src.is_format_lab())
    {
      SELENE_ASSERT(out.nr_bytes_per_channel == 1);
      buf = convert_lab_to_rgb_interleaved(buf, nr_bytes_read, src.width, rows_in_this_strip, lab_converter);
      nr_bytes_read = static_cast<tmsize_t>(buf.size());
    }
    else if (src.is_format_grayscale())
    {
      if (src.bits_per_sample == 1)
      {
        buf = impl::tiff::convert_single_channel_1bit_to_8bit(buf, nr_bytes_read, src.width, rows_in_this_strip);
        nr_bytes_read = static_cast<tmsize_t>(buf.size());
      }
      else if (src.bits_per_sample == 4)
      {
        buf = impl::tiff::convert_single_channel_4bit_to_8bit(buf, nr_bytes_read, src.width, rows_in_this_strip);
        nr_bytes_read = static_cast<tmsize_t>(buf.size());
      }
    }

    std::uint8_t* buf_begin = buf.data();
    std::uint8_t* buf_end = buf.data() + nr_bytes_read;

    if (src.inverted())
    {
      std::for_each(buf_begin, buf_end, [](auto& px){
        px = static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() - px); });
    }

    // Copy buffer into target image. Data is stored interleaved.

    // This strip starts at row (strip_index * rows_per_strip) in the output image.
    const auto y_start = sln::to_pixel_index(strip_index * strip_layout.rows_per_strip);

    const auto nr_rows_remaining_in_output_image = dyn_img_view.height() - y_start;
    const auto bytes_remaining_in_output_image = nr_rows_remaining_in_output_image * dyn_img_view.row_bytes();

    // Make sure we do not write past the end of the allocated output image.
    const auto max_bytes_to_write = std::min(static_cast<std::size_t>(nr_bytes_read),
                                             static_cast<std::size_t>(bytes_remaining_in_output_image));

    if (max_bytes_to_write < static_cast<std::size_t>(nr_bytes_read))
    {
      message_log.add(
          "Writing fewer bytes than expected to target image (max_bytes_to_write = "
          + std::to_string(max_bytes_to_write) + ", nr_bytes_read = " + std::to_string(nr_bytes_read) + ")\n",
          MessageType::Warning);
    }

    // Write to output image row-wise (since it might not be packed)
    auto remaining_bytes_to_write = std::ptrdiff_t{bytes_remaining_in_output_image};
    for (auto y = PixelIndex{0}; y < to_pixel_index(rows_in_this_strip); ++y)
    {
      if (remaining_bytes_to_write < 0)
      {
        break;
      }

      const auto dst_ptr = dyn_img_view.byte_ptr(PixelIndex{y + y_start});
      const auto src_ptr = buf.data() + y * dyn_img_view.row_bytes();
      const auto sz = std::min(static_cast<std::size_t>(dyn_img_view.row_bytes()),
                               static_cast<std::size_t>(remaining_bytes_to_write));
      std::memcpy(dst_ptr, src_ptr, sz);

      remaining_bytes_to_write -= dyn_img_view.row_bytes();
    }
  }

  return true;
}

bool read_data_strips_planar(TIFF* tif,
                             const sln::TiffImageLayout& src,
                             const sln::impl::tiff::ImageLayoutStrips& strip_layout,
                             const sln::impl::tiff::YCbCrInfo& /*ycbcr_info*/,
                             const sln::impl::tiff::YCbCrConverter& /*ycbcr_converter*/,
                             const sln::impl::tiff::LabConverter& /*lab_converter*/,
                             const sln::impl::tiff::OutputLayout& out,
                             sln::MutableDynImageView& dyn_img_view,
                             sln::MessageLog& message_log)
{
  if (src.is_format_ycbcr())
  {
    message_log.add("Cannot read TIFF image with the following properties: strips, planar, YCbCr (not implemented).",
                    MessageType::Error);
    return false;
  }
  else if (src.is_format_lab())
  {
    message_log.add("Cannot read TIFF image with the following properties: strips, planar, Lab (not implemented).",
                    MessageType::Error);
    return false;
  }

  std::vector<std::uint8_t> buf(static_cast<std::size_t>(strip_layout.size_bytes));

  const auto nr_planes_per_sample = strip_layout.nr_strips / src.samples_per_pixel;

  for (tstrip_t strip_index = 0; strip_index < strip_layout.nr_strips; ++strip_index)
  {
    // First, identify which of the planes we're dealing with here:
    const auto channel_index = strip_index / nr_planes_per_sample;
    // Now, identify which strip we're at for the respective plane (channel).
    const auto plane_strip_index = strip_index % nr_planes_per_sample;

    auto nr_bytes_read = TIFFReadEncodedStrip(tif, strip_index, buf.data(), -1);
    SELENE_ASSERT(nr_bytes_read <= static_cast<std::ptrdiff_t>(buf.size()));

    if (nr_bytes_read < 0)
    {
      message_log.add("Strip " + std::to_string(strip_index) + ": nr_bytes_read == " + std::to_string(nr_bytes_read),
                      MessageType::Error);
      return false;
    }

    const auto expected_nr_bytes = (strip_layout.rows_per_strip * to_unsigned(out.width) * src.bits_per_sample) >> 3;
    const auto rows_in_this_strip = strip_layout.rows_per_strip * nr_bytes_read / expected_nr_bytes;

    if (nr_bytes_read != static_cast<tmsize_t>(expected_nr_bytes)
        && plane_strip_index != static_cast<tstrip_t>(nr_planes_per_sample - 1))
    {
      message_log.add(
          "Strip " + std::to_string(strip_index)
          + "nr_bytes_read (" + std::to_string(nr_bytes_read) + ") != expected_nr_bytes ("
          + std::to_string(expected_nr_bytes) + ")", MessageType::Warning);
    }

    std::uint8_t* const buf_begin = buf.data();
    std::uint8_t* const buf_end = buf.data() + nr_bytes_read;

    if (src.inverted())
    {
      std::for_each(buf_begin, buf_end, [](auto& px){
        px = static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() - px); });
    }

    // Copy buffer into target image. Data is stored in separate planes.
    SELENE_ASSERT(nr_bytes_read % out.nr_bytes_per_channel == 0);

    const auto nr_bytes_per_input_row = out.width * out.nr_bytes_per_channel;

    for (auto y = PixelIndex{0}; y < to_pixel_index(rows_in_this_strip); ++y)
    {
      auto buf_ptr = buf_begin + y * nr_bytes_per_input_row;
      SELENE_ASSERT(buf_ptr + nr_bytes_per_input_row <= buf_end);

      const auto row_y = to_pixel_index(plane_strip_index * strip_layout.rows_per_strip + to_unsigned(y));
      auto img_ptr = dyn_img_view.byte_ptr(row_y);

      impl::tiff::copy_samples(buf_ptr, to_unsigned(out.width), channel_index, out.nr_bytes_per_channel, out.nr_channels, img_ptr);
    }
  }

  return true;
}

sln::impl::tiff::OutputLayout get_output_layout(TIFF* tif,
                                                const sln::TiffImageLayout& src,
                                                const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                                                const sln::impl::tiff::ImageLayoutStrips strip_layout,
                                                MessageLog& message_log)
{
    if (src.planar_config == TIFFPlanarConfig::Separate)
  {
    SELENE_ASSERT(strip_layout.nr_strips % src.samples_per_pixel == 0);
  }

  const auto spp = (src.planar_config == TIFFPlanarConfig::Contiguous) ? 1 : src.samples_per_pixel;
  const auto nr_strips_per_sample = src.height / strip_layout.rows_per_strip
                                    + ((src.height % strip_layout.rows_per_strip) ? 1 : 0);

  // do some sanity checks that strip layout is as expected
  for (uint16 sample = 0; sample < spp; ++sample)
  {
    for (uint32 row = 0; row < src.height; ++row)
    {
      [[maybe_unused]] const auto strip_index = TIFFComputeStrip(tif, row, sample);
      [[maybe_unused]] const auto expected_strip_index = sample * nr_strips_per_sample + (row / strip_layout.rows_per_strip);
      SELENE_ASSERT(strip_index == expected_strip_index);
    }
  }

  if (src.is_format_ycbcr())
  {
    SELENE_ASSERT(src.samples_per_pixel == 3);
    ycbcr_info.check_strip_size(src.width, src.height, strip_layout.rows_per_strip, message_log);
  }

  // --------------- Output to sln::ImageData<>:

  // Allocate data for the whole target image

  // The minimum bit depth is 8 (1 byte/channel)
  const auto nr_bytes_per_channel_out = std::max(std::uint16_t{1}, static_cast<std::uint16_t>(src.bits_per_sample >> 3));
  sln::impl::tiff::OutputLayout out(to_pixel_length(src.width), to_pixel_length(src.height),
                                    to_signed(src.samples_per_pixel), to_signed(nr_bytes_per_channel_out),
                                    impl::tiff::photometric_to_pixel_format(src.photometric, src.samples_per_pixel),
                                    impl::tiff::sample_format_to_sample_format(src.sample_format));

  return out;
}

} // namespace _


template <typename DynImageOrView>
bool read_data_strips(TIFF* tif,
                      const sln::TiffImageLayout& src,
                      const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                      const sln::impl::tiff::YCbCrConverter& ycbcr_converter,
                      const sln::impl::tiff::LabConverter& lab_converter,
                      DynImageOrView& dyn_img_or_view,
                      sln::MessageLog& message_log)
{
  const auto nr_rows_per_strip = std::min(src.height, impl::tiff::get_field<uint32>(tif, TIFFTAG_ROWSPERSTRIP));
  const sln::impl::tiff::ImageLayoutStrips strip_layout(TIFFNumberOfStrips(tif), TIFFStripSize(tif), nr_rows_per_strip);

  const auto out = get_output_layout(tif, src, ycbcr_info, strip_layout, message_log);

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
    message_log.add("Cannot prepare input image or view; most likely it is a view that cannot be resized.", MessageType::Error);
    return false;
  }

  if (src.planar_config == TIFFPlanarConfig::Contiguous)
  {
    return read_data_strips_interleaved(tif, src, strip_layout, ycbcr_info, ycbcr_converter, lab_converter, out, dyn_img_or_view.view(), message_log);
  }
  else  // planar_config == TIFFPlanarConfig::Separate
  {
    return read_data_strips_planar(tif, src, strip_layout, ycbcr_info, ycbcr_converter, lab_converter, out, dyn_img_or_view.view(), message_log);
  }
}

// Explicit instantiations:
template bool read_data_strips(TIFF*,
                               const sln::TiffImageLayout&,
                               const sln::impl::tiff::YCbCrInfo&,
                               const sln::impl::tiff::YCbCrConverter&,
                               const sln::impl::tiff::LabConverter&,
                               sln::DynImage&,
                               sln::MessageLog&);
template bool read_data_strips(TIFF*,
                               const sln::TiffImageLayout&,
                               const sln::impl::tiff::YCbCrInfo&,
                               const sln::impl::tiff::YCbCrConverter&,
                               const sln::impl::tiff::LabConverter&,
                               sln::MutableDynImageView&,
                               sln::MessageLog&);

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)