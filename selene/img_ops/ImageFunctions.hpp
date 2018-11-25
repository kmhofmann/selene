// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_IMAGE_FUNCTIONS_HPP
#define SELENE_IMG_OPS_IMAGE_FUNCTIONS_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <stdexcept>
#include <type_traits>

namespace sln {

template <typename PixelType>
bool allocate(Image<PixelType>& img_dst, TypedLayout layout)
{
  const bool did_reallocate = img_dst.reallocate(layout, guess_row_alignment(reinterpret_cast<std::uintptr_t>(img_dst.byte_ptr()), img_dst.stride_bytes()));
  return did_reallocate;

  //layout.stride_bytes = std::max(layout.stride_bytes, Stride{PixelTraits<PixelType>::nr_bytes * layout.width});
  //const auto nr_bytes_to_allocate = layout.stride_bytes * layout.height;
  //const auto nr_currently_allocated_bytes = DEPRECATED_img.stride_bytes() * DEPRECATED_img.height();

  //auto commit_new_geometry = [=]() {
  //  width_ = width;
  //  height_ = height;
  //  stride_bytes_ = stride_bytes;
  //};

  //// No need to act, if size parameters match
  //const auto bytes_match = shrink_to_fit ? (nr_bytes_to_allocate == nr_currently_allocated_bytes)
  //                                       : (nr_bytes_to_allocate <= nr_currently_allocated_bytes);
  //if (!force_allocation && bytes_match && owns_memory_)
  //{
  //  commit_new_geometry();
  //  return;
  //}

  //commit_new_geometry();

  //deallocate_bytes_if_owned();
  //allocate_bytes(nr_bytes_to_allocate, base_alignment_bytes);
}

template <typename Derived>
bool maybe_allocate(ImageBase<Derived>& img_dst, const TypedLayout& layout)
{
  if (img_dst.width() == layout.width && img_dst.height() == layout.height)
  {
    return false;
  }

  if constexpr(ImageBase<Derived>::is_view)
  {
    throw std::runtime_error("Cannot resize image view.");
  }

  return allocate(img_dst.derived(), layout);

  //constexpr auto base_alignment_bytes = Image<PixelType>::default_base_alignment_;
  //constexpr auto shrink_to_fit = true;
  //constexpr auto force_allocation = false;
  //constexpr auto allow_view_reallocation = false;
  ////allocate(width, height, stride_bytes, base_alignment_bytes, shrink_to_fit, force_allocation, allow_view_reallocation);
}

template <typename PixelType, typename Derived>
void fill(ImageBase<Derived>& img_dst, PixelType value)
{
  for (PixelIndex y = 0_idx; y < img_dst.height(); ++y)
  {
    std::fill(img_dst.data(y), img_dst.data_row_end(y), value);
  }
}

// ---

template <typename DerivedSrc>
auto view(const ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc>
auto view(ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc>
auto view(const ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelType, ImageModifiability::Constant>(byte_ptr, layout);
}

template <typename DerivedSrc>
auto view(ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;
  constexpr auto modifiability = ImageBase<DerivedSrc>::modifiability();

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelType, modifiability>(byte_ptr, layout);
}

// ---

// TODO: make this an internal function
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

// ---

template <typename DerivedSrc, typename DerivedDst>
void clone(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
{
  static_check_copy_compatibility(img_src, img_dst);

  maybe_allocate(img_dst, img_src.layout());
  copy_rows_from(img_src, img_dst);
}

template <typename DerivedSrc, typename DerivedDst>
void clone(const ImageBase<DerivedSrc>& img_src, const BoundingBox& region_src, ImageBase<DerivedDst>& img_dst)
{
  static_check_copy_compatibility(img_src, img_dst);

  const auto view_src = view(img_src, region_src);
  maybe_allocate(img_dst, view_src.layout());
  copy_rows_from(view_src, img_dst);
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

// ---

template <typename DerivedSrcDst>
void crop(ImageBase<DerivedSrcDst>& img, const BoundingBox& region)
{
  auto cropped_clone = clone(img, region);
  img = std::move(cropped_clone);
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_IMAGE_FUNCTIONS_HPP
