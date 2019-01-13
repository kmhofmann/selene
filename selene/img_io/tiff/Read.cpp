// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/img_io/tiff/Read.hpp>

#include <selene/base/Assert.hpp>
#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/MemoryReader.hpp>

#include <selene/img_io/tiff/_impl/TIFFDetail.hpp>
#include <selene/img_io/tiff/_impl/TIFFIOFunctions.hpp>
#include <selene/img_io/tiff/_impl/TIFFReadStrips.hpp>
#include <selene/img_io/tiff/_impl/TIFFReadTiles.hpp>

#include <selene/selene_export.hpp>

#include <type_traits>
#include <tuple>

#include <tiff.h>
#include <tiffio.h>


namespace sln {

namespace {

TiffImageLayout get_tiff_layout(TIFF* tif)
{
  using namespace impl::tiff;
  const auto width = get_field<uint32>(tif, TIFFTAG_IMAGEWIDTH);
  const auto height = get_field<uint32>(tif, TIFFTAG_IMAGELENGTH);
  const auto depth = get_field<uint32>(tif, TIFFTAG_IMAGEDEPTH, 1);

  const auto samples_per_pixel = get_field<uint16>(tif, TIFFTAG_SAMPLESPERPIXEL);
  const auto bits_per_sample = get_field<uint16>(tif, TIFFTAG_BITSPERSAMPLE);
  const auto planar_config = get_field<uint16>(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  const auto photometric = get_field<uint16>(tif, TIFFTAG_PHOTOMETRIC);
  const auto sample_format = get_field<uint16>(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
  const auto compression = get_field<uint16>(tif, TIFFTAG_COMPRESSION);

  return TiffImageLayout(width, height, depth, samples_per_pixel, bits_per_sample,
                         planar_config_lib_to_pub(planar_config),
                         photometric_tag_lib_to_pub(photometric),
                         sample_format_lib_to_pub(sample_format),
                         compression_lib_to_pub(compression));
}

struct ConversionStructures
{
  impl::tiff::YCbCrInfo ycbcr_info;
  impl::tiff::YCbCrConverter ycbcr_converter;
  impl::tiff::LabConverter lab_converter;
};

ConversionStructures
get_tiff_color_conversion_structures(TIFF* tif)
{
  using namespace impl::tiff;
  auto reference_blackwhite = get_field<float*>(tif, TIFFTAG_REFERENCEBLACKWHITE);
  auto ycbcr_coefficients = get_field<float*>(tif, TIFFTAG_YCBCRCOEFFICIENTS);
  const auto ycbcr_coefficients_red = ycbcr_coefficients[0];
  const auto ycbcr_coefficients_green = ycbcr_coefficients[1];
  const auto ycbcr_coefficients_blue = ycbcr_coefficients[2];
  const auto ycbcr_positioning = get_field<uint16>(tif, TIFFTAG_YCBCRPOSITIONING);
  const auto ycbcr_subsampling = get_field_2<uint16>(tif, TIFFTAG_YCBCRSUBSAMPLING);
  const auto ycbcr_subsampling_horz = ycbcr_subsampling.first;
  const auto ycbcr_subsampling_vert = ycbcr_subsampling.second;
  auto white_point_coefficients = get_field<float*>(tif, TIFFTAG_WHITEPOINT);

  YCbCrInfo ycbcr_info(ycbcr_coefficients_red, ycbcr_coefficients_green, ycbcr_coefficients_blue,
                           ycbcr_positioning, ycbcr_subsampling_horz, ycbcr_subsampling_vert);
  YCbCrConverter ycbcr_converter(ycbcr_coefficients, reference_blackwhite);
  LabConverter lab_converter(white_point_coefficients);

  return ConversionStructures{ycbcr_info, std::move(ycbcr_converter), std::move(lab_converter)};
}


bool check_suitability(const TiffImageLayout& layout, MessageLog& message_log)
{
  // Reading of 3D data is unsupported.
  if (layout.depth != 1)
  {
    message_log.add("TIFF reader: 3D data (depth != 1) is unsupported.", MessageType::Error);
    return false;
  }

  SELENE_FORCED_ASSERT(layout.planar_config == TIFFPlanarConfig::Contiguous || layout.planar_config == TIFFPlanarConfig::Separate);

  if (layout.photometric == TIFFPhotometricTag::YCbCr && !(layout.samples_per_pixel == 3 && layout.bits_per_sample == 8))
  {
    message_log.add("TIFF reader: YCbCr data layout is unsupported..", MessageType::Error);
    return false;
  }

  if (layout.photometric == TIFFPhotometricTag::Palette)
  {
    message_log.add("TIFF reader: Palette images unsupported", MessageType::Error);
    return false;
  }

  if (layout.bits_per_sample != 1 && layout.bits_per_sample != 4 && layout.bits_per_sample != 8 && layout.bits_per_sample != 16)
  {
    message_log.add(
        "TIFF reader: Bit depth " + std::to_string(layout.bits_per_sample) +
        " unsupported (bits per sample have to be: 1, 4, 8 or 16).", MessageType::Error);
    return false;
  }

  // Support only grayscale, RGB(A), or YCbCr inputs for now (i.e. no Lab, etc.)
  const auto pixel_format = impl::tiff::photometric_to_pixel_format(layout.photometric, layout.samples_per_pixel);
  if (pixel_format != sln::PixelFormat::Y && pixel_format != sln::PixelFormat::RGB
      && pixel_format != sln::PixelFormat::RGBA && pixel_format != sln::PixelFormat::YCbCr)
  {
    message_log.add(
        "TIFF reader: Photometric tag '" + impl::tiff::photometric_to_string(layout.photometric) +
        "' unsupported (has to be one of: grayscale, RGB(A), YCbCr).", MessageType::Error);
    return false;
  }

  return true;
}

}  // namespace _

template <typename SourceType>
struct TIFFReadObject<SourceType>::Impl
{
  TIFF* tif{nullptr};
  impl::tiff::SourceStruct<SourceType> ss;

  void open_read(SourceType& source)
  {
    close();

    ss = impl::tiff::SourceStruct{&source};
    tif = TIFFClientOpen("", "r",
                         reinterpret_cast<thandle_t>(&ss),
                         impl::tiff::r_read_func<SourceType>,
                         impl::tiff::r_write_func<SourceType>,
                         impl::tiff::r_seek_func<SourceType>,
                         impl::tiff::r_close_func<SourceType>,
                         impl::tiff::r_size_func<SourceType>,
                         impl::tiff::r_map_func<SourceType>,
                         impl::tiff::r_unmap_func<SourceType>);
    SELENE_ASSERT(tif != nullptr);
  }

  void open_read(SourceType&& source)
  {
    open_read(source);
  }

  void close()
  {
    if (tif != nullptr)
    {
      TIFFClose(tif);
    }
  }
};

template <typename SourceType>
TIFFReadObject<SourceType>::TIFFReadObject()
    : impl_(std::make_unique<TIFFReadObject::Impl>())
{
}

template <typename SourceType>
TIFFReadObject<SourceType>::~TIFFReadObject()
{
  impl_->close();
}

template <typename SourceType>
bool TIFFReadObject<SourceType>::open(SourceType& source)
{
  impl_->open_read(source);
  return impl_->tif != nullptr;
}

template <typename SourceType>
bool TIFFReadObject<SourceType>::open(SourceType&& source)
{
  impl_->open_read(std::forward<SourceType>(source));
  return impl_->tif != nullptr;
}

template <typename SourceType>
TiffImageLayout TIFFReadObject<SourceType>::get_layout()
{
  return get_tiff_layout(impl_->tif);
}

template <typename SourceType>
bool TIFFReadObject<SourceType>::advance_directory()
{
  return TIFFReadDirectory(impl_->tif) != 0;
}

template <typename SourceType>
int TIFFReadObject<SourceType>::set_directory(std::uint16_t index)
{
  return TIFFSetDirectory(impl_->tif, index);
}

// Explicit instantiations:
template class SELENE_EXPORT TIFFReadObject<FileReader>;
template class SELENE_EXPORT TIFFReadObject<MemoryReader>;


namespace impl {

template <typename SourceType, typename DynImageOrView>
bool tiff_read_current_directory(TIFFReadObject<SourceType>& tiff_obj,
                                 MessageLog& message_log,
                                 DynImageOrView& dyn_img_or_view)
{
  auto tif = tiff_obj.impl_->tif;

  // Obtain several structures containing information about the image.
  auto layout = get_tiff_layout(tif);
  const auto& cs = get_tiff_color_conversion_structures(tif);

  // Check if we can (likely) read the image
  bool suitable = check_suitability(layout, message_log);
  if (!suitable)
  {
    return false;
  }

//  message_log.add(str(oss() << layout), MessageType::Message);

  if (layout.photometric == TIFFPhotometricTag::YCbCr)
  {
    // Yay! This is the kind of stuff you only can find hidden deep in the libtiff source code somewhere...
    // ...but is vital to read respectively stored images (e.g. quad-jpeg.tif) correctly.
    if (layout.planar_config == TIFFPlanarConfig::Contiguous && layout.compression == TIFFCompression::JPEG)
    {
      TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
      layout.photometric = TIFFPhotometricTag::RGB;
    }
  }

  const bool read_successfully = [&, tif]() {
    if (TIFFIsTiled(tif) == 0)
    {
      return impl::read_data_strips(tif, layout, cs.ycbcr_info, cs.ycbcr_converter, cs.lab_converter, dyn_img_or_view, message_log);
    }
    else
    {
      return impl::read_data_tiles(tif, layout, cs.ycbcr_info, cs.ycbcr_converter, cs.lab_converter, dyn_img_or_view, message_log);
    }
  }();

  return read_successfully;
}

// Explicit instantiations:
template SELENE_EXPORT bool tiff_read_current_directory(TIFFReadObject<FileReader>&, MessageLog&, DynImage&);
template SELENE_EXPORT bool tiff_read_current_directory(TIFFReadObject<FileReader>&, MessageLog&, MutableDynImageView&);

template SELENE_EXPORT bool tiff_read_current_directory(TIFFReadObject<MemoryReader>&, MessageLog&, DynImage&);
template SELENE_EXPORT bool tiff_read_current_directory(TIFFReadObject<MemoryReader>&, MessageLog&, MutableDynImageView&);

}  // namespace impl

}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)