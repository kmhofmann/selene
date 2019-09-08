// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_IMPL_TIFF_READ_HIGH_LEVEL_HPP
#define SELENE_IMG_IO_IMPL_TIFF_READ_HIGH_LEVEL_HPP

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <selene/base/MessageLog.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img_io/tiff/Common.hpp>

#include <tiff.h>
#include <tiffio.h>

namespace sln::impl {

bool read_data_high_level(TIFF* tif,
                          const sln::TiffImageLayout& src,
                          sln::DynImage<>& dyn_img,
                          sln::MessageLog& message_log);

}  // namespace sln::impl

#endif  // defined(SELENE_WITH_LIBTIFF)

#endif  // SELENE_IMG_IO_IMPL_TIFF_READ_HIGH_LEVEL_HPP
