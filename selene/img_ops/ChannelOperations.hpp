// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CHANNEL_OPERATIONS_HPP
#define SELENE_IMG_OPS_CHANNEL_OPERATIONS_HPP

/// @file

#include <selene/base/Assert.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/Utilities.hpp>

#include <selene/img_ops/Clone.hpp>

namespace sln {

template <typename ImgSrc, typename ImgDst>
void inject_channels(const ImgSrc& src, ImgDst& dst, std::size_t dst_start_channel);

template <sln::PixelFormat pixel_format = sln::PixelFormat::Unknown, typename... Imgs>
auto stack_images(Imgs... imgs);

// ----------
// Implementation:

/** \brief Copies all channel(s) of the source image to the specified channel(s) of the target image.
 *
 * The channels [dst_start_channel, ..., dst_start_channel + nr_channels(src) - 1] of the target image will be modified.
 *
 * The function throws an exception either if there are too many source channels to copy, i.e. they don't fit into the
 * target image, or if the two images are not already the same size.
 *
 * @tparam ImgSrc The image type of the source image.
 * @tparam ImgDst The image type of the target image.
 * @param src The source image.
 * @param dst The target image.
 * @param dst_start_channel The channel where to start inserting the source channels.
 */
template <typename ImgSrc, typename ImgDst>
void inject_channels(const ImgSrc& src, ImgDst& dst, std::size_t dst_start_channel)
{
  static_assert(is_image_type_v<ImgSrc> && is_image_type_v<ImgDst>,
                "Need to supply a typed image (owning or view) as input/output argument to inject_channels");

  constexpr auto nr_channels_src = sln::PixelTraits<typename ImgSrc::PixelType>::nr_channels;
  constexpr auto nr_channels_dst = sln::PixelTraits<typename ImgDst::PixelType>::nr_channels;

  if (dst_start_channel + nr_channels_src > nr_channels_dst)
  {
    throw std::runtime_error("inject_channels: Source channels do not fit in target image using specified starting channel.");
  }

  if (dst.width() != src.width() || dst.height() != src.height())
  {
    throw std::runtime_error("inject_channels: Images are not the same size.");
  }

  const auto w = src.width();
  const auto h = src.height();

  for (sln::PixelIndex y{0}; y < h; ++y)
  {
    const auto* ptr_src = src.data(y);
    auto* ptr_dst = dst.data(y);

    for (sln::PixelIndex x{0}; x < w; ++x)
    {
      for (std::size_t c = 0; c < nr_channels_src; ++c) // c is known at compile time
      {
        (*ptr_dst)[dst_start_channel + c] = (*ptr_src)[c];
      }

      ++ptr_src;
      ++ptr_dst;
    }
  }
}

namespace impl
{
  template <typename T, typename Func>
  auto apply_min(Func func, T&& val)
  {
    return func(val);
  }

  template <typename T0, typename T1, typename... Ts, typename Func>
  auto apply_min(Func func, T0&& val1, T1&& val2, Ts&&... vs)
  {
    return (func(val1) < func(val2))
      ? apply_min(func, val1, std::forward<Ts>(vs)...)
      : apply_min(func, val2, std::forward<Ts>(vs)...);
  }

  template <typename T, typename Func>
  auto apply_max(Func func, T&& val)
  {
    return func(val);
  }

  template <typename T0, typename T1, typename... Ts, typename Func>
  auto apply_max(Func func, T0&& val1, T1&& val2, Ts&&... vs)
  {
    return (func(val1) > func(val2))
      ? apply_max(func, val1, std::forward<Ts>(vs)...)
      : apply_max(func, val2, std::forward<Ts>(vs)...);
  }

  template <typename Img, typename... Imgs>
  struct ElementType {
    // Just pick type of first one; TODO: can we do better?
    using type = typename sln::PixelTraits<typename Img::PixelType>::Element;
  };

  template <typename... Imgs> using ElementType_t = typename ElementType<Imgs...>::type;

  template <typename ImgSrc, typename ImgDst>
  void inject_channels_rec(ImgDst& img_dst, std::size_t dst_start_channel, const ImgSrc& img_src)
  {
    inject_channels(img_src, img_dst, dst_start_channel);
  }

  template <typename ImgSrc, typename... ImgsSrc, typename ImgDst>
  void inject_channels_rec(ImgDst& img_dst, std::size_t dst_start_channel, const ImgSrc& img_src, ImgsSrc&&... imgs_src)
  {
    constexpr auto nr_channels_src = sln::PixelTraits<typename ImgSrc::PixelType>::nr_channels;
    constexpr auto nr_channels_dst = sln::PixelTraits<typename ImgDst::PixelType>::nr_channels;

    if (dst_start_channel + nr_channels_src >= nr_channels_dst)
    {
      return;
    }

    inject_channels(img_src, img_dst, dst_start_channel);
    inject_channels_rec(img_dst, dst_start_channel + nr_channels_src, std::forward<ImgsSrc>(imgs_src)...);
  }

} // namespace impl

template <sln::PixelFormat pixel_format = sln::PixelFormat::Unknown, typename Img>
auto stack_images(const Img& img)
{
  return sln::clone(img);
}

/** \brief Stacks the channels of the specified images, and returns the concatenated output image.
 *
 * The number of channels of the returned image will be equal to the cumulative number of channels of the input
 * images.
 *
 * All input images have to be of the same size; otherwise, an exception is thrown.
 *
 * @tparam pixel_format The desired pixel format of the output image.
 * @tparam Imgs The image types of the input images (parameter pack).
 * @param imgs The input images (parameter pack).
 * @return The concatenated output image.
 */
template <sln::PixelFormat pixel_format, typename... Imgs>
auto stack_images(Imgs... imgs)
{
  using T = impl::ElementType_t<Imgs...>;
  constexpr auto nr_channels = count_nr_channels(std::forward<Imgs>(imgs)...);

  // Determine minimum width and height of common image
  const auto min_width = impl::apply_min([](const auto& img){ return img.width(); }, imgs...);
  const auto min_height = impl::apply_min([](const auto& img){ return img.height(); }, imgs...);
  const auto width = impl::apply_max([](const auto& img){ return img.width(); }, imgs...);
  const auto height = impl::apply_max([](const auto& img){ return img.height(); }, imgs...);
  using PixelType = sln::Pixel<T, nr_channels, pixel_format>;

  sln::Image<PixelType> img_dst({width, height});

  if (width > min_width || height > min_height)
  {
    throw std::runtime_error("stack_images: Images are not all the same size.");
  }

  impl::inject_channels_rec(img_dst, 0, std::forward<Imgs>(imgs)...);
  return img_dst;
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_CHANNEL_OPERATIONS_HPP

