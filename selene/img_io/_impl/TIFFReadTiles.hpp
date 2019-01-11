// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_IMPL_TIFF_READ_TILES_HPP
#define SELENE_IMG_IO_IMPL_TIFF_READ_TILES_HPP

#if defined(SELENE_WITH_LIBTIFF)

#include <tiff.h>
#include <tiffio.h>

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img_io/TIFFCommon.hpp>
#include <selene/img_io/_impl/TIFFDetail.hpp>

namespace sln {
namespace impl {

template <typename DynImageOrView>
bool read_data_tiles(TIFF* tif,
                     const sln::TiffImageLayout& src,
                     const sln::impl::tiff::YCbCrInfo& ycbcr_info,
                     const sln::impl::tiff::YCbCrConverter& /*ycbcr_converter*/,
                     const sln::impl::tiff::LabConverter& /*lab_converter*/,
                     DynImageOrView& dyn_img_or_view,
                     sln::MessageLog& message_log);

}  // namespace impl
}  // namespace sln

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_IMPL_TIFF_READ_TILES_HPP