// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CONVOLUTION_HPP
#define SELENE_IMG_OPS_CONVOLUTION_HPP

/// @file

#include <selene/base/Kernel.hpp>
#include <selene/base/Promote.hpp>
#include <selene/base/Round.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <selene/img/typed/access/BorderAccessors.hpp>

#include <selene/img_ops/Allocate.hpp>

#include <algorithm>
#include <type_traits>

namespace sln {

template <BorderAccessMode access_mode = BorderAccessMode::Replicated,
          typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_x(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel);

template <BorderAccessMode access_mode = BorderAccessMode::Replicated,
          typename DerivedSrc, typename DerivedDst, typename KernelValueType, KernelSize kernel_size>
void convolution_y(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                   const Kernel<KernelValueType, kernel_size>& kernel);

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
    const auto val = kernel[k_idx] * px;
    sum = sum + val;
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
    const auto val = kernel[k_idx] * px;
    sum = sum + val;
  }

  return sum;
}

}  // namespace impl

// ---

template <BorderAccessMode access_mode,
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

  for (auto y = 0_idx; y < img_dst.height(); ++y)
  {
    auto x = 0_idx;

    for (; x < x_left; ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }

    for (; x < x_right; ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, BorderAccessMode::Unchecked>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }

    for (; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_x<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  }
}

template <BorderAccessMode access_mode,
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

  auto y = 0_idx;

  for (; y < y_top; ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  }

  for (; y < y_bottom; ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, BorderAccessMode::Unchecked>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  }

  for (; y < img_dst.height(); ++y)
  {
    for (auto x = 0_idx; x < img_dst.width(); ++x)
    {
      const auto res = impl::convolve_pixels_y<ConvolutionResultType, access_mode>(img_src, x, y, kernel, k_offset);
      img_dst(x, y) = sln::round<ElementTypeDst>(res);
    }
  }
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_CONVOLUTION_HPP
