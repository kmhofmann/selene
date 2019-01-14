// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/tiff/Write.hpp>

#include <selene/base/Assert.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img_io/tiff/_impl/TIFFDetail.hpp>
#include <selene/img_io/tiff/_impl/TIFFIOFunctions.hpp>

#include <selene/selene_export.hpp>

#include <array>
#include <type_traits>

#include <tiff.h>
#include <tiffio.h>

namespace sln {

namespace {

void set_tiff_layout(TIFF* tif, const ConstantDynImageView& view, const TIFFWriteOptions& write_options)
{
  using namespace impl::tiff;
  set_field<uint32>(tif, TIFFTAG_IMAGEWIDTH, static_cast<uint32>(view.width()));
  set_field<uint32>(tif, TIFFTAG_IMAGELENGTH, static_cast<uint32>(view.height()));
  set_field<uint32>(tif, TIFFTAG_IMAGEDEPTH, uint32{1});

  set_field<uint16>(tif, TIFFTAG_SAMPLESPERPIXEL, static_cast<uint16>(view.nr_channels()));
  set_field<uint16>(tif, TIFFTAG_BITSPERSAMPLE, static_cast<uint16>(view.nr_bytes_per_channel() * 8));
  set_field<uint16>(tif, TIFFTAG_PHOTOMETRIC, pixel_format_to_photometric(view.pixel_format()));
  set_field<uint16>(tif, TIFFTAG_SAMPLEFORMAT, sample_format_to_sample_format(view.sample_format()));

  if (view.pixel_format() == PixelFormat::RGBA)
  {
    // We need to specify the extra sample.
    std::array<uint16, 1> extra_sample_types = {{EXTRASAMPLE_ASSOCALPHA}};
    set_field<uint16>(tif, TIFFTAG_EXTRASAMPLES, 1, extra_sample_types.data());
  }

  set_field<uint16>(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);  // we only write interleaved data
  set_field<uint16>(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  set_field<uint16>(tif, TIFFTAG_COMPRESSION, compression_pub_to_lib(write_options.compression_type));
  if (write_options.compression_type == TIFFCompression::JPEG)
  {
    set_field<int>(tif, TIFFTAG_JPEGQUALITY, write_options.jpeg_quality);
  }

  set_field<const char*>(tif, TIFFTAG_SOFTWARE, "Selene");
}

void set_tiff_layout_strips(TIFF* tif, std::size_t nr_rows_per_strip)
{
  using namespace impl::tiff;
  set_field<std::uint32_t>(tif, TIFFTAG_ROWSPERSTRIP, static_cast<std::uint32_t>(nr_rows_per_strip));
}

void set_tiff_layout_tiles(TIFF* tif, std::size_t tile_width, std::size_t tile_height)
{
  using namespace impl::tiff;
  SELENE_ASSERT(tile_width % 8 == 0 && tile_height % 8 == 0);
  set_field<uint32>(tif, TIFFTAG_TILEWIDTH, static_cast<uint32>(tile_width));
  set_field<uint32>(tif, TIFFTAG_TILELENGTH, static_cast<uint32>(tile_height));
  set_field<uint32>(tif, TIFFTAG_TILEDEPTH, uint32{1});
}

bool check_tiff_tile_size(TIFF* tif, const ConstantDynImageView& /*view*/, TIFFWriteOptions& opts, MessageLog& /*message_log*/)
{
  auto tw = static_cast<uint32>(opts.tile_width);
  auto th = static_cast<uint32>(opts.tile_height);
  TIFFDefaultTileSize(tif, &tw, &th);
  opts.tile_width = static_cast<std::size_t>(tw);
  opts.tile_height = static_cast<std::size_t>(th);

  return true;
}

}  // namespace _

template <typename SinkType>
struct TIFFWriteObject<SinkType>::Impl
{
  TIFF* tif{nullptr};
  impl::tiff::SinkStruct<SinkType> ss;

  void open_write(SinkType& sink)
  {
    close();

    ss = impl::tiff::SinkStruct{&sink};
    tif = TIFFClientOpen("", "wm",
                         reinterpret_cast<thandle_t>(&ss),
                         impl::tiff::w_read_func<SinkType>,
                         impl::tiff::w_write_func<SinkType>,
                         impl::tiff::w_seek_func<SinkType>,
                         impl::tiff::w_close_func<SinkType>,
                         impl::tiff::w_size_func<SinkType>,
                         nullptr, nullptr);
    SELENE_ASSERT(tif != nullptr);
  }

  void open_write(SinkType&& sink)
  {
    open_write(sink);
  }

  void close()
  {
    if (tif != nullptr)
    {
      TIFFClose(tif);
      tif = nullptr;
    }
  }
};

template <typename SinkType>
TIFFWriteObject<SinkType>::TIFFWriteObject() : impl_(std::make_unique<TIFFWriteObject::Impl>())
{
}

template <typename SinkType>
TIFFWriteObject<SinkType>::~TIFFWriteObject()
{
  impl_->close();
}

template <typename SinkType>
bool TIFFWriteObject<SinkType>::open(SinkType& sink)
{
  impl_->open_write(sink);
  return impl_->tif != nullptr;
}

template <typename SinkType>
bool TIFFWriteObject<SinkType>::open(SinkType&& sink)
{
  impl_->open_write(std::forward<SinkType>(sink));
  return impl_->tif != nullptr;
}

template <typename SinkType>
bool TIFFWriteObject<SinkType>::flush()
{
  if (impl_->tif != nullptr)
  {
    int res = TIFFFlush(impl_->tif);
    return res != 0;
  }

  return false;
}

// Explicit instantiations:
template class SELENE_EXPORT TIFFWriteObject<FileWriter>;
template class SELENE_EXPORT TIFFWriteObject<VectorWriter>;


namespace impl {

bool tiff_write_to_current_directory_strips(TIFF* tif, const TIFFWriteOptions& write_options, MessageLog& /*message_log*/, const ConstantDynImageView& view)
{
  const auto height = view.height();
  const auto row_size_bytes = to_unsigned(view.row_bytes());
  const auto nr_rows_per_strip = [&write_options, row_size_bytes]() {
    // For JPEG compression, the nr of rows per strip must be a multiple of 8.
    const auto nrps = std::size_t(write_options.max_bytes_per_strip / row_size_bytes);
    return std::min(write_options.nr_rows_per_strip, std::max(std::size_t{8}, nrps - (nrps % 8)));
  }();

  set_tiff_layout_strips(tif, nr_rows_per_strip);

  const auto nr_strips = std::size_t{to_unsigned(height) / nr_rows_per_strip + ((to_unsigned(height) % nr_rows_per_strip > 0) ? 1 : 0)};
  const auto rows_in_last_strip = (to_unsigned(height) % nr_rows_per_strip > 0) ? (to_unsigned(height) % nr_rows_per_strip) : (nr_rows_per_strip);
  const auto expected_strip_size = TIFFStripSize(tif);

//  std::cout << "nr_rows_per_strip = " << nr_rows_per_strip << '\n';
//  std::cout << "height / nr_rows_per_strip = " << height / nr_rows_per_strip << '\n';
//  std::cout << "height % nr_rows_per_strip = " << height % nr_rows_per_strip << '\n';
//  std::cout << "nr_strips = " << nr_strips << '\n';
//  std::cout << "expected_strip_size = " << expected_strip_size << '\n';

  const bool packed = view.is_packed();
  std::vector<std::uint8_t> buffer(packed ? std::size_t{0} : static_cast<std::size_t>(expected_strip_size));

  for (std::size_t strip_idx = 0; strip_idx < nr_strips; ++strip_idx)
  {
    const auto cur_row = strip_idx * nr_rows_per_strip;
    const auto nr_available_rows = (strip_idx < nr_strips - 1) ? nr_rows_per_strip : rows_in_last_strip;

    const auto buf_size = static_cast<tmsize_t>(nr_available_rows * row_size_bytes);
    SELENE_ASSERT(buf_size <= expected_strip_size);

    const auto buf_ptr = [packed, cur_row, nr_available_rows, row_size_bytes, &view, &buffer]() -> const std::uint8_t* {
      // If image data is packed, return direct pointer to it.
      if (packed)
      {
        return view.byte_ptr(to_pixel_index(cur_row));
      }

      // Otherwise, copy non-packed image data into contiguous strip buffer.
      for (auto row_idx = std::size_t{0}; row_idx < nr_available_rows; ++row_idx)
      {
        auto dst = buffer.data() + row_idx * row_size_bytes;
        std::memcpy(dst, view.byte_ptr(to_pixel_index(cur_row + row_idx)), row_size_bytes);
      }
      return buffer.data();
    }();

    tstrip_t strip = TIFFComputeStrip(tif, static_cast<uint32>(cur_row), 0);
    SELENE_ASSERT(static_cast<std::size_t>(strip) == strip_idx);
//    std::cout << "cur_row = " << cur_row << ", nr_available_rows = " << nr_available_rows << ", buf_size = " << buf_size << ", strip = " << strip << '\n';

    auto size_written = TIFFWriteEncodedStrip(tif, strip, const_cast<void*>(static_cast<const void*>(buf_ptr)), buf_size);

    if (size_written < 0)
    {
      return false;
    }
  }

  return true;
}

bool tiff_write_to_current_directory_tiles(TIFF* tif, const TIFFWriteOptions& write_options, MessageLog& message_log, const ConstantDynImageView& view)
{
  using value_type = PixelIndex::value_type;

  const auto tile_width = write_options.tile_width;
  const auto tile_height = write_options.tile_height;
  set_tiff_layout_tiles(tif, tile_width, tile_height);

  const auto width = static_cast<value_type>(view.width());
  const auto height = static_cast<value_type>(view.height());
  const auto nr_bytes_per_pixel = view.layout().nr_bytes_per_pixel();

  // Allocate temporary buffer
  std::vector<std::uint8_t> buffer(tile_width * tile_height * to_unsigned(nr_bytes_per_pixel));

  // For each tile...
  uint32 tile_ctr = 0;
  for (auto src_y = 0_idx; src_y < height; src_y += static_cast<value_type>(tile_height))
  {
    for (auto src_x = 0_idx; src_x < width; src_x += static_cast<value_type>(tile_width))
    {
      const auto x = static_cast<uint32>(src_x);
      const auto y = static_cast<uint32>(src_y);
      uint16 sample = 0;
      const auto tile_idx = TIFFComputeTile(tif, x, y, uint32{0}, sample);
      SELENE_ASSERT(tile_idx == tile_ctr); // ???

      // std::fill(buffer.begin(), buffer.end(), std::uint8_t{0});

      // Compute buffer size
      const auto this_tile_width = std::min(width - src_x, static_cast<value_type>(tile_width));
      const auto this_tile_height = std::min(height - src_y, static_cast<value_type>(tile_height));
      const auto nr_bytes_per_this_tile_row = static_cast<std::size_t>(this_tile_width * nr_bytes_per_pixel);

      // Copy region to buffer
      for (auto tile_y = 0; tile_y < this_tile_height; ++tile_y)
      {
        auto dst = buffer.data() + static_cast<std::size_t>(tile_y) * tile_width * static_cast<std::size_t>(nr_bytes_per_pixel);
        SELENE_ASSERT(dst + tile_width * static_cast<std::size_t>(nr_bytes_per_pixel) <= buffer.data() + buffer.size());
        auto src = view.byte_ptr(to_pixel_index(src_x), to_pixel_index(src_y + tile_y));
        std::memcpy(dst, src, nr_bytes_per_this_tile_row);
      }

      const auto tile_written_size = TIFFWriteEncodedTile(tif, tile_idx, buffer.data(), static_cast<tmsize_t>(buffer.size()));

      if (tile_written_size == -1)
      {
        message_log.add("Tile " + std::to_string(tile_idx) + " could not be written.", MessageType::Error);
        return false;
      }

      SELENE_ASSERT(tile_written_size == static_cast<tsize_t>(buffer.size()));

      ++tile_ctr;
    }
  }

  return true;
}

template <typename SinkType, typename DynImageOrView>
bool tiff_write_to_current_directory(TIFFWriteObject<SinkType>& tiff_obj, const TIFFWriteOptions& write_options, MessageLog& message_log, const DynImageOrView& dyn_img_or_view)
{
  auto tif = tiff_obj.impl_->tif;
  const auto view = dyn_img_or_view.constant_view();

  set_tiff_layout(tif, view, write_options);

  if (write_options.layout == TIFFWriteOptions::Layout::Strips)
  {
    return tiff_write_to_current_directory_strips(tif, write_options, message_log, view);
  }
  else
  {
    TIFFWriteOptions local_write_options = write_options;
    const bool size_ok = check_tiff_tile_size(tif, view, local_write_options, message_log);

    if (!size_ok)
    {
      return false;
    }

    return tiff_write_to_current_directory_tiles(tif, local_write_options, message_log, view);
  }
}

// Explicit instantiations:
template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<FileWriter>&, const TIFFWriteOptions&, MessageLog&, const DynImage&);
template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<FileWriter>&, const TIFFWriteOptions&, MessageLog&, const ConstantDynImageView&);
template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<FileWriter>&, const TIFFWriteOptions&, MessageLog&, const MutableDynImageView&);

template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<VectorWriter>&, const TIFFWriteOptions&, MessageLog&, const DynImage&);
template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<VectorWriter>&, const TIFFWriteOptions&, MessageLog&, const ConstantDynImageView&);
template SELENE_EXPORT bool tiff_write_to_current_directory(TIFFWriteObject<VectorWriter>&, const TIFFWriteOptions&, MessageLog&, const MutableDynImageView&);

}  // namespace impl

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)