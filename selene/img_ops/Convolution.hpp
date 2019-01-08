// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CONVOLUTION_HPP
#define SELENE_IMG_OPS_CONVOLUTION_HPP

/// @file

#include <selene/base/Kernel.hpp>
#include <selene/base/Promote.hpp>
#include <selene/base/Round.hpp>

#include <selene/img/pixel/Pixel.hpp>
#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <selene/img/typed/access/BorderAccessors.hpp>

#include <selene/img_ops/Allocate.hpp>

#include <algorithm>
#include <type_traits>

namespace sln {

template <BorderAccessMode access_mode, std::size_t shift_right = 0,
          typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_x(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel);

template <BorderAccessMode access_mode, std::size_t shift_right = 0,
    typename DerivedSrc, typename KernelValueType, KernelSize kernel_size>
Image<typename DerivedSrc::PixelType> convolution_x(const ImageBase<DerivedSrc>& img_src, const Kernel<KernelValueType, kernel_size>& kernel);

template <BorderAccessMode access_mode, std::size_t shift_right = 0,
          typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_y(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel);

template <BorderAccessMode access_mode, std::size_t shift_right = 0,
    typename DerivedSrc, typename KernelValueType, KernelSize kernel_size>
Image<typename DerivedSrc::PixelType> convolution_y(const ImageBase<DerivedSrc>& img_src, const Kernel<KernelValueType, kernel_size>& kernel);

// ----------
// Implementation:

namespace impl {

template <typename ConvolutionResultType, BorderAccessMode access_mode, typename DerivedSrc,
          typename KernelValueType, KernelSize kernel_size>
auto convolve_pixels_x(const ImageBase<DerivedSrc>& img_src, PixelIndex x, PixelIndex y,
                       const Kernel<KernelValueType, kernel_size>& kernel,
                       PixelIndex::value_type k_offset)
{
  auto sum = PixelTraits<ConvolutionResultType>::zero_element;

  for (auto k_idx = std::size_t{0}; k_idx < kernel.size(); ++k_idx)
  {
    const auto x_idx = PixelIndex{x + static_cast<PixelIndex::value_type>(k_idx) - k_offset};
    const auto& px = ImageBorderAccessor<access_mode>::access(img_src, x_idx, y);
    sum += kernel[k_idx] * px;
  }

  return sum;
}

template <typename ConvolutionResultType, BorderAccessMode access_mode, typename DerivedSrc,
    typename KernelValueType, KernelSize kernel_size>
auto convolve_pixels_y(const ImageBase<DerivedSrc>& img_src, PixelIndex x, PixelIndex y,
                       const Kernel<KernelValueType, kernel_size>& kernel,
                       PixelIndex::value_type k_offset)
{
  auto sum = PixelTraits<ConvolutionResultType>::zero_element;

  for (auto k_idx = std::size_t{0}; k_idx < kernel.size(); ++k_idx)
  {
    const auto y_idx = PixelIndex{y + static_cast<PixelIndex::value_type>(k_idx) - k_offset};
    const auto& px = ImageBorderAccessor<access_mode>::access(img_src, x, y_idx);
    sum += kernel[k_idx] * px;
  }

  return sum;
}

}  // namespace impl

// ---

/** \brief Performs a convolution in x-direction for each pixel of the input image; i.e. with a (1xN) kernel.
 *
 * @tparam access_mode The border access mode to be used when going outside the image bounds.
 * @tparam shift_right An optional bit-shift factor, to be applied before each convolution result is written to the
 *                     output image. `0` by default. Non-zero values are useful in combination with a respectively
 *                     scaled integer kernel.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @tparam KernelValueType The value type of the kernel elements (usually automatically deduced).
 * @tparam kernel_size The kernel size (usually automatically deduced).
 * @param img_src The typed source image.
 * @param img_dst The typed target image.
 * @param kernel The kernel to apply.
 */
template <BorderAccessMode access_mode, std::size_t shift_right,
          typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_x(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel)
{
  using namespace sln::literals;

  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeDst = typename ImageBase<DerivedDst>::PixelType;
  constexpr auto nr_channels = PixelTraits<PixelTypeSrc>::nr_channels;
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels);

  using ElementTypeSrc = typename PixelTraits<PixelTypeSrc>::Element;
  using ElementTypeDst = typename PixelTraits<PixelTypeDst>::Element;

  using ConvolutionResultType = Pixel<std::common_type_t<ElementTypeSrc, KernelValueType>, nr_channels, PixelTraits<PixelTypeDst>::pixel_format>;

  allocate(img_dst, img_src.layout());
  const auto k_offset = (static_cast<PixelIndex::value_type>(kernel.size()) - 1) / 2;
  const auto x_left = k_offset;
  const auto x_right = img_src.width() - k_offset;

  auto write_to_dst = [&img_dst](auto res, auto x, auto y) {
    if constexpr (/*std::is_integral_v<ConvolutionResultType> &&*/ shift_right > 0)
    {
      img_dst(x, y) = (res + (1 << (shift_right - 1))) >> shift_right;
    }
    else
    {
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  };

  for (auto y = 0_idx; y < img_dst.height(); ++y)
  {
    auto x = 0_idx;

    for (; x < x_left; ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }

    for (; x < x_right; ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, BorderAccessMode::Unchecked>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }

    for (; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }
  }
}

/** \brief Performs a convolution in x-direction for each pixel of the input image; i.e. with a (1xN) kernel.
 *
 * @tparam access_mode The border access mode to be used when going outside the image bounds.
 * @tparam shift_right An optional bit-shift factor, to be applied before each convolution result is written to the
 *                     output image. `0` by default. Non-zero values are useful in combination with a respectively
 *                     scaled integer kernel.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam KernelValueType The value type of the kernel elements (usually automatically deduced).
 * @tparam kernel_size The kernel size (usually automatically deduced).
 * @param img_src The typed source image.
 * @param img_dst The typed target image.
 * @param kernel The kernel to apply.
 * @return The output image with the applied convolution.
 */
template <BorderAccessMode access_mode, std::size_t shift_right,
    typename DerivedSrc, typename KernelValueType, KernelSize kernel_size>
Image<typename DerivedSrc::PixelType> convolution_x(const ImageBase<DerivedSrc>& img_src, const Kernel<KernelValueType, kernel_size>& kernel)
{
  Image<typename DerivedSrc::PixelType> img_dst;
  convolution_x<access_mode, shift_right>(img_src, img_dst, kernel);
  return img_dst;
}

/** \brief Performs a convolution in y-direction for each pixel of the input image; i.e. with a (Nx1) kernel.
 *
 * @tparam access_mode The border access mode to be used when going outside the image bounds.
 * @tparam shift_right An optional bit-shift factor, to be applied before each convolution result is written to the
 *                     output image. `0` by default. Non-zero values are useful in combination with a respectively
 *                     scaled integer kernel.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @tparam KernelValueType The value type of the kernel elements (usually automatically deduced).
 * @tparam kernel_size The kernel size (usually automatically deduced).
 * @param img_src The typed source image.
 * @param img_dst The typed target image.
 * @param kernel The kernel to apply.
 */
template <BorderAccessMode access_mode, std::size_t shift_right,
    typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_y(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel)
{
  using namespace sln::literals;

  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeDst = typename ImageBase<DerivedDst>::PixelType;
  constexpr auto nr_channels = PixelTraits<PixelTypeSrc>::nr_channels;
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels);

  using ElementTypeSrc = typename PixelTraits<PixelTypeSrc>::Element;
  using ElementTypeDst = typename PixelTraits<PixelTypeDst>::Element;

  using ConvolutionResultType = Pixel<std::common_type_t<ElementTypeSrc, KernelValueType>, nr_channels, PixelTraits<PixelTypeDst>::pixel_format>;

  allocate(img_dst, img_src.layout());
  const auto k_offset = (static_cast<PixelIndex::value_type>(kernel.size()) - 1) / 2;
  const auto y_top = k_offset;
  const auto y_bottom = img_src.height() - k_offset;

  auto write_to_dst = [&img_dst](auto res, auto x, auto y) {
    if constexpr (/*std::is_integral_v<ConvolutionResultType> &&*/ shift_right > 0)
    {
      img_dst(x, y) = (res + (1 << (shift_right - 1))) >> shift_right;
    }
    else
    {
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  };

  auto y = 0_idx;

  for (; y < y_top; ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }
  }

  for (; y < y_bottom; ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, BorderAccessMode::Unchecked>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }
  }

  for (; y < img_dst.height(); ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      write_to_dst(res, x, y);
    }
  }
}

/** \brief Performs a convolution in y-direction for each pixel of the input image; i.e. with a (Nx1) kernel.
 *
 * @tparam access_mode The border access mode to be used when going outside the image bounds.
 * @tparam shift_right An optional bit-shift factor, to be applied before each convolution result is written to the
 *                     output image. `0` by default. Non-zero values are useful in combination with a respectively
 *                     scaled integer kernel.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam KernelValueType The value type of the kernel elements (usually automatically deduced).
 * @tparam kernel_size The kernel size (usually automatically deduced).
 * @param img_src The typed source image.
 * @param img_dst The typed target image.
 * @param kernel The kernel to apply.
 * @return The output image with the applied convolution.
 */
template <BorderAccessMode access_mode, std::size_t shift_right,
    typename DerivedSrc, typename KernelValueType, KernelSize kernel_size>
Image<typename DerivedSrc::PixelType> convolution_y(const ImageBase<DerivedSrc>& img_src, const Kernel<KernelValueType, kernel_size>& kernel)
{
  Image<typename DerivedSrc::PixelType> img_dst;
  convolution_y<access_mode, shift_right>(img_src, img_dst, kernel);
  return img_dst;
}


}  // namespace sln

#endif  // SELENE_IMG_OPS_CONVOLUTION_HPP
