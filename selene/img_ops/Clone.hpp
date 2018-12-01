// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CLONE_HPP
#define SELENE_IMG_OPS_CLONE_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <selene/img_ops/Allocate.hpp>

namespace sln {

namespace impl {

template <typename DerivedSrc, typename DerivedDst>
void static_check_copy_compatibility(const ImageBase<DerivedSrc>& /*img_src*/, ImageBase<DerivedDst>& /*img_dst*/)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeDst = typename ImageBase<DerivedDst>::PixelType;

  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<std::remove_cv_t<typename PixelTraits<PixelTypeSrc>::Element>,
                    std::remove_cv_t<typename PixelTraits<PixelTypeDst>::Element>>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");
}

template <typename DerivedSrc, typename DerivedDst>
void copy_rows_from(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
{
  static_assert(ImageBase<DerivedDst>::is_modifiable);
  static_check_copy_compatibility(img_src, img_dst);

  SELENE_ASSERT(img_src.data() && img_dst.data());
  SELENE_ASSERT(img_src.width() == img_dst.width() && img_src.height() == img_dst.height());

  for (PixelIndex y = 0_idx; y < img_dst.height(); ++y)
  {
    std::copy(img_src.data(y), img_src.data_row_end(y), img_dst.data(y));
  }
}

}  // namespace impl


template <typename DerivedSrc, typename DerivedDst>
void clone(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
{
  impl::static_check_copy_compatibility(img_src, img_dst);

  allocate(img_dst, img_src.layout());
  impl::copy_rows_from(img_src, img_dst);
}

template <typename DerivedSrc, typename DerivedDst>
void clone(const ImageBase<DerivedSrc>& img_src, const BoundingBox& region_src, ImageBase<DerivedDst>& img_dst)
{
  impl::static_check_copy_compatibility(img_src, img_dst);

  const auto view_src = view(img_src, region_src);
  allocate(img_dst, view_src.layout());
  impl::copy_rows_from(view_src, img_dst);
}

template <typename DerivedSrc>
Image<typename DerivedSrc::PixelType> clone(const ImageBase<DerivedSrc>& img_src)
{
  using PixelType = typename DerivedSrc::PixelType;
  Image<PixelType> img_dst(img_src.layout());
  clone(img_src, img_dst);
  return img_dst;
}

template <typename DerivedSrc>
Image<typename DerivedSrc::PixelType> clone(const ImageBase<DerivedSrc>& img_src, const BoundingBox& region_src)
{
  using PixelType = typename DerivedSrc::PixelType;
  Image<PixelType> img_dst(img_src.layout());
  clone(img_src, region_src, img_dst);
  return img_dst;
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_CLONE_HPP
