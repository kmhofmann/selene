// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_DYN_VIEW_HPP
#define SELENE_IMG_OPS_DYN_VIEW_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>
#include <selene/img/common/DataPtr.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <type_traits>

namespace sln {

/// \addtogroup group-img-ops
/// @{

template <ImageModifiability modifiability>
auto view(const DynImageView<modifiability>& dyn_img) -> DynImageView<modifiability>;

auto view(const DynImage& dyn_img) -> ConstantDynImageView;

auto view(DynImage& dyn_img) -> MutableDynImageView;

template <typename PixelType, ImageModifiability modifiability>
auto view(const DynImageView<modifiability>& dyn_img, const BoundingBox& region) -> DynImageView<modifiability>;

template <typename PixelType>
auto view(const DynImage& dyn_img, const BoundingBox& region) -> ConstantDynImageView;

template <typename PixelType>
auto view(DynImage& dyn_img, const BoundingBox& region) -> MutableDynImageView;

// TODO: add overloads where pixel format (semantics?) may change?

// ----------
// Implementation:

/** \brief Create a non-owning view onto the specified dynamic image view.
 *
 * This overload essentially results in a no-op, but is provided for completeness and usage in generic code.
 *
 * @tparam modifiability Determines whether image contents are constant or mutable.
 * @param dyn_img The input image view.
 * @return A non-owning view onto the input image view.
 */
template <ImageModifiability modifiability>
auto view(const DynImageView<modifiability>& dyn_img) -> DynImageView<modifiability>
{
  return dyn_img.view();
}

/** \brief Create a non-owning constant view onto the specified dynamic image.
 *
 * @param dyn_img The input image.
 * @return A non-owning constant view onto the input image.
 */
inline auto view(const DynImage& dyn_img) -> ConstantDynImageView
{
  return dyn_img.constant_view();
}

/** \brief Create a non-owning mutable view onto the specified dynamic image.
 *
 * @param dyn_img The input image.
 * @return A non-owning mutable view onto the input image.
 */
inline auto view(DynImage& dyn_img) -> MutableDynImageView
{
  return dyn_img.view();
}

/** \brief Create a non-owning view onto the respective sub-region of the specified dynamic image view.
 *
 * @tparam PixelType The designated pixel type, to uniquely specify the sub-region.
 * @tparam modifiability Determines whether image contents are constant or mutable.
 * @param dyn_img The input image.
 * @param region The sub-region to create a view of.
 * @return A non-owning view onto the sub-region of the input image view.
 */
template <typename PixelType, ImageModifiability modifiability>
auto view(const DynImageView<modifiability>& dyn_img, const BoundingBox& region) -> DynImageView<modifiability>
{
  const auto data_offset = Bytes(dyn_img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = dyn_img.byte_ptr() + data_offset;
  const auto layout = UntypedLayout{region.width(),
                                    region.height(),
                                    dyn_img.nr_channels(),
                                    dyn_img.nr_bytes_per_channel(),
                                    dyn_img.stride_bytes()};
  return DynImageView<modifiability>(byte_ptr, layout, dyn_img.semantics());
}

/** \brief Create a non-owning constant view onto the respective sub-region of the specified dynamic image.
 *
 * @tparam PixelType The designated pixel type, to uniquely specify the sub-region.
 * @param dyn_img The input image.
 * @param region The sub-region to create a view of.
 * @return A non-owning constant view onto the sub-region of the input image.
 */
template <typename PixelType>
auto view(const DynImage& dyn_img, const BoundingBox& region) -> ConstantDynImageView
{
  const auto data_offset = Bytes(dyn_img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = dyn_img.byte_ptr() + data_offset;
  const auto layout = UntypedLayout{region.width(),
                                    region.height(),
                                    dyn_img.nr_channels(),
                                    dyn_img.nr_bytes_per_channel(),
                                    dyn_img.stride_bytes()};
  return ConstantDynImageView(byte_ptr, layout, dyn_img.semantics());
}

/** \brief Create a non-owning mutable view onto the respective sub-region of the specified dynamic image.
 *
 * @tparam PixelType The designated pixel type, to uniquely specify the sub-region.
 * @param dyn_img The input image.
 * @param region The sub-region to create a view of.
 * @return A non-owning mutable view onto the sub-region of the input image.
 */
template <typename PixelType>
auto view(DynImage& dyn_img, const BoundingBox& region) -> MutableDynImageView
{
  const auto data_offset = Bytes(dyn_img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = dyn_img.byte_ptr() + data_offset;
  const auto layout = UntypedLayout{region.width(),
                                    region.height(),
                                    dyn_img.nr_channels(),
                                    dyn_img.nr_bytes_per_channel(),
                                    dyn_img.stride_bytes()};
  return MutableDynImageView(byte_ptr, layout, dyn_img.semantics());
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_DYN_VIEW_HPP
