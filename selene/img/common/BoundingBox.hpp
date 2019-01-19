// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_COMMON_BOUNDING_BOX_HPP
#define SELENE_IMG_COMMON_BOUNDING_BOX_HPP

/// @file

#include <selene/img/common/Types.hpp>

#include <algorithm>

namespace sln {

/// \addtogroup group-img-common
/// @{

/** \brief Represents an axis-aligned, rectangular bounding box, to describe a sub-part of an image.
 *
 * Represents a bounding box by its top-left corner and its width and height parameters.
 */
class BoundingBox
{
public:
  constexpr BoundingBox() noexcept;
  constexpr BoundingBox(PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height) noexcept;
  constexpr BoundingBox(PixelIndex x0, PixelIndex y0, PixelIndex x1, PixelIndex y1) noexcept;

  constexpr PixelIndex x0() const noexcept;
  constexpr PixelIndex y0() const noexcept;
  constexpr PixelLength width() const noexcept;
  constexpr PixelLength height() const noexcept;

  constexpr PixelIndex x1() const noexcept;
  constexpr PixelIndex y1() const noexcept;

  constexpr bool empty() const noexcept;
  constexpr void sanitize(PixelLength max_img_width, PixelLength max_img_height) noexcept;

private:
  PixelIndex x0_;
  PixelIndex y0_;
  PixelLength width_;
  PixelLength height_;
};

/** Default constructor. Constructs a bounding box of size (0, 0) top-left corner (0, 0).
 */
constexpr BoundingBox::BoundingBox() noexcept
    : x0_(PixelIndex{0}), y0_(PixelIndex{0}), width_(PixelLength{0}), height_(PixelLength{0})
{
}

/** Constructs a bounding box of size (width, height) with top-left corner (x0, y0).
 *
 * \param x0 x-coordinate of the top-left box corner.
 * \param y0 y-coordinate of the top-left box corner.
 * \param width Box width.
 * \param height Box height.
 */
constexpr BoundingBox::BoundingBox(PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height) noexcept
    : x0_(x0), y0_(y0), width_(width), height_(height)
{
  SELENE_ASSERT(width_ > 0);
  SELENE_ASSERT(height_ > 0);
}

/** Constructs a bounding box of from top-left corner (x0, y0) and bottom-right corner (x1, y1).
 *
 * Note that the bottom right coordinates are *exclusive*, so, for example, a box from identical top-left corner (x, y)
 * and bottom-right corner (x, y) will have width and height (0, 0).
 * Width is computed as (x1 - x0), and height is computed similarly as (y1 - y0).
 *
 * \param x0 x-coordinate of the top-left box corner.
 * \param y0 y-coordinate of the top-left box corner.
 * \param x1 x-coordinate of the bottom-right box corner.
 * \param y1 y-coordinate of the bottom-right box corner.
 */
constexpr BoundingBox::BoundingBox(PixelIndex x0, PixelIndex y0, PixelIndex x1, PixelIndex y1) noexcept
    : x0_(x0), y0_(y0), width_(x1 - x0), height_(y1 - y0)
{
  SELENE_ASSERT(width_ > 0);
  SELENE_ASSERT(height_ > 0);
}

/** Returns the x-coordinate of the top-left corner, i.e. the x-coordinate of the left box side.
 *
 * \return x-coordinate of the top-left corner.
 */
constexpr PixelIndex BoundingBox::x0() const noexcept
{
  return x0_;
}

/** Returns the y-coordinate of the top-left corner, i.e. the y-coordinate of the top box side.
 *
 * \return y-coordinate of the top-left corner.
 */
constexpr PixelIndex BoundingBox::y0() const noexcept
{
  return y0_;
}

/** Returns the width of the bounding box.
 *
 * \return Box width.
 */
constexpr PixelLength BoundingBox::width() const noexcept
{
  return width_;
}

/** Returns the height of the bounding box.
 *
 * \return Box height.
 */
constexpr PixelLength BoundingBox::height() const noexcept
{
  return height_;
}

/** Returns the x-coordinate of the bottom-right corner, i.e. the x-coordinate of the right box side.
 *
 * This coordinate, like y1(), is *exclusive*; i.e. is not part of the bounding box itself.
 *
 * \return x-coordinate of the bottom-right corner.
 */
constexpr PixelIndex BoundingBox::x1() const noexcept
{
  return PixelIndex{x0_ + width_};
}

/** Returns the y-coordinate of the bottom-right corner, i.e. the y-coordinate of the bottom box side.
 *
 * This coordinate, like x1(), is *exclusive*; i.e. is not part of the bounding box itself.
 *
 * \return y-coordinate of the bottom-right corner.
 */
constexpr PixelIndex BoundingBox::y1() const noexcept
{
  return PixelIndex{y0_ + height_};
}

/** Returns true, if the bounding box has 0 width or height; false otherwise.
 *
 * \return True, if box is empty, false otherwise.
 */
constexpr bool BoundingBox::empty() const noexcept
{
  return width_ == PixelLength{0} || height_ == PixelLength{0};
}

/** Sanitizes bounding box w.r.t. given maximum image extents.
 *
 * Modifies bounding box width and height such that the resulting bounding box does not go outside of the maximum
 * image bounds.
 *
 * @param max_img_width Maximum image width.
 * @param max_img_height Maximum image height.
 */
constexpr void BoundingBox::sanitize(PixelLength max_img_width, PixelLength max_img_height) noexcept
{
  x0_ = std::max(x0_, PixelIndex{0});
  y0_ = std::max(y0_, PixelIndex{0});
  width_ = std::min(width_, PixelLength{max_img_width - x0_});
  height_ = std::min(height_, PixelLength{max_img_height - y0_});
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_COMMON_BOUNDING_BOX_HPP
