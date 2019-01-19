// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_RESAMPLE_HPP
#define SELENE_IMG_OPS_RESAMPLE_HPP

/// @file

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/access/Interpolators.hpp>

#include <selene/img_ops/Allocate.hpp>
#include <selene/img_ops/Clone.hpp>

#include <cmath>

namespace sln {

/// \addtogroup group-img-ops
/// @{

template <ImageInterpolationMode interpolation_mode = ImageInterpolationMode::Bilinear, typename DerivedSrc>
auto resample(const ImageBase<DerivedSrc>& img, PixelLength new_width, PixelLength new_height)
    -> Image<typename ImageBase<DerivedSrc>::PixelType>;

template <ImageInterpolationMode interpolation_mode = ImageInterpolationMode::Bilinear, typename DerivedSrcDst>
void resample(const ImageBase<DerivedSrcDst>& img_src, PixelLength new_width, PixelLength new_height, ImageBase<DerivedSrcDst>& img_dst);

// ----------
// Implementation:

namespace impl {

template <typename Func, typename FuncSafe, typename DerivedDst>
void apply_resample_functions(Func func, FuncSafe func_safe,
                              default_float_t dst_to_src_factor_x,
                              default_float_t dst_to_src_factor_y,
                              PixelLength safe_boundary_left,
                              PixelLength safe_boundary_right,
                              PixelLength safe_boundary_top,
                              PixelLength safe_boundary_bottom,
                              ImageBase<DerivedDst>& img_dst)
{
  const auto dst_width = img_dst.width();
  const auto dst_height = img_dst.height();

  const auto bound_left = PixelIndex{safe_boundary_left};
  const auto bound_right = PixelIndex{safe_boundary_right};
  const auto bound_top = PixelIndex{safe_boundary_top};
  const auto bound_bottom = PixelIndex{safe_boundary_bottom};

  for (auto y_dst = 0_idx; y_dst < bound_top; ++y_dst)
  {
    const auto y_src = y_dst * dst_to_src_factor_y;

    for (auto x_dst = 0_idx; x_dst < dst_width; ++x_dst)
    {
      const auto x_src = x_dst * dst_to_src_factor_x;
      const auto value = func_safe(x_src, y_src);
      img_dst(x_dst, y_dst) = value;
    }
  }

  for (auto y_dst = bound_top; y_dst < bound_bottom; ++y_dst)
  {
    const auto y_src = y_dst * dst_to_src_factor_y;

    for (auto x_dst = 0_idx; x_dst < bound_left; ++x_dst)
    {
      const auto x_src = x_dst * dst_to_src_factor_x;
      const auto value = func_safe(x_src, y_src);
      img_dst(x_dst, y_dst) = value;
    }

    for (auto x_dst = bound_left; x_dst < bound_right; ++x_dst)
    {
      const auto x_src = x_dst * dst_to_src_factor_x;
      const auto value = func(x_src, y_src);
      img_dst(x_dst, y_dst) = value;
    }

    for (auto x_dst = bound_right; x_dst < dst_width; ++x_dst)
    {
      const auto x_src = x_dst * dst_to_src_factor_x;
      const auto value = func_safe(x_src, y_src);
      img_dst(x_dst, y_dst) = value;
    }
  }

  for (auto y_dst = bound_bottom; y_dst < dst_height; ++y_dst)
  {
    const auto y_src = y_dst * dst_to_src_factor_y;

    for (auto x_dst = 0_idx; x_dst < dst_width; ++x_dst)
    {
      const auto x_src = x_dst * dst_to_src_factor_x;
      const auto value = func_safe(x_src, y_src);
      img_dst(x_dst, y_dst) = value;
    }
  }

}

}  // namespace impl


/** \brief Resamples the input image pixels to fit the output image dimensions, using the specified interpolation mode.
 *
 * This function only samples the respective pixels in the input image. No low-pass filtering is performed to limit the
 * frequency range; therefore, aliasing may occur when shrinking the image dimensions.
 *
 * @tparam interpolation_mode The interpolation mode to use.
 * @tparam DerivedSrc The typed source image type.
 * @param img The input image to be resampled
 * @param new_width The width of the target image.
 * @param new_height The height of the target image
 * @return The sampled target image.
 */
template <ImageInterpolationMode interpolation_mode, typename DerivedSrc>
auto resample(const ImageBase<DerivedSrc>& img, PixelLength new_width, PixelLength new_height)
    -> Image<typename ImageBase<DerivedSrc>::PixelType>
{
  Image<typename ImageBase<DerivedSrc>::PixelType> img_dst;
  resample<interpolation_mode>(img, new_width, new_height, img_dst);
  return img_dst;
}

/** \brief Resamples the input image pixels to fit the output image dimensions, using the specified interpolation mode.
 *
 * This function only samples the respective pixels in the input image. No low-pass filtering is performed to limit the
 * frequency range; therefore, aliasing may occur when shrinking the image dimensions.
 *
 * @tparam interpolation_mode The interpolation mode to use.
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img_src The input image to be resampled.
 * @param img_dst The sampled target image.
 * @param new_width The width of the target image.
 * @param new_height The height of the target image
 */
template <ImageInterpolationMode interpolation_mode, typename DerivedSrcDst>
void resample(const ImageBase<DerivedSrcDst>& img_src, PixelLength new_width, PixelLength new_height, ImageBase<DerivedSrcDst>& img_dst)
{
  if (&img_src == &img_dst)
  {
    return;
  }

  if (new_width == img_src.width() && new_height == img_src.height())
  {
    clone(img_src, img_dst);
    return;
  }

  allocate(img_dst, {new_width, new_height});

  const auto dst_to_src_factor_x = img_src.width() / static_cast<default_float_t>(new_width);
  const auto dst_to_src_factor_y = img_src.height() / static_cast<default_float_t>(new_height);

  const auto safe_boundary_left = to_pixel_length(
      std::ceil(ImageInterpolator<interpolation_mode>::index_to_left / dst_to_src_factor_x));
  const auto safe_boundary_right = to_pixel_length(
      std::ceil(ImageInterpolator<interpolation_mode>::index_to_right / dst_to_src_factor_x));
  const auto safe_boundary_top = to_pixel_length(
      std::ceil(ImageInterpolator<interpolation_mode>::index_to_up / dst_to_src_factor_y));
  const auto safe_boundary_bottom = to_pixel_length(
      std::ceil(ImageInterpolator<interpolation_mode>::index_to_down / dst_to_src_factor_y));

  const auto func = [&img_src](auto x, auto y) {
    return ImageInterpolator<interpolation_mode, BorderAccessMode::Unchecked>::interpolate(img_src, x, y);
  };

  const auto func_safe = [&img_src](auto x, auto y) {
    return ImageInterpolator<interpolation_mode, BorderAccessMode::Replicated>::interpolate(img_src, x, y);
  };

  impl::apply_resample_functions(func, func_safe, dst_to_src_factor_x, dst_to_src_factor_y,
                                 safe_boundary_left, safe_boundary_right, safe_boundary_top, safe_boundary_bottom,
                                 img_dst);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_RESAMPLE_HPP
