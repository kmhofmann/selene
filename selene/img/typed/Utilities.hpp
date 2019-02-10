// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_UTILITIES_HPP
#define SELENE_IMG_TYPED_UTILITIES_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/_impl/StaticChecks.hpp>

namespace sln {

/// \addtogroup group-img-typed
/// @{

template <typename Img>
constexpr
std::ptrdiff_t count_nr_channels(const Img&)
{
  impl::static_assert_is_image_or_view<Img>();

  using PixelType = typename Img::PixelType;
  return sln::PixelTraits<PixelType>::nr_channels;
}

/** \brief Returns the cumulative number of channels of the supplied image(s).
 *
 * @tparam Img An image type.
 * @tparam Imgs Image types (parameter pack).
 * @param img An image.
 * @param imgs Parameter pack of zero or more images.
 * @return The cumulative number of channels.
 */
template <typename Img, typename... Imgs>
constexpr
std::ptrdiff_t count_nr_channels(const Img& img, Imgs&&... imgs)
{
  return count_nr_channels(img) + count_nr_channels(std::forward<Imgs>(imgs)...);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_UTILITIES_HPP
