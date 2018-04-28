// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_BOUNDING_BOX_HPP
#define SELENE_IMG_BOUNDING_BOX_HPP

/// @file

#include <selene/img/Types.hpp>

namespace sln {

/** \brief Represents an axis-aligned, rectangular bounding box, to describe a sub-part of an image.
 *
 * Represents a bounding box by its top-left corner and its width and height parameters.
 */
class BoundingBox
{
public:
  BoundingBox() noexcept;
  BoundingBox(PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height) noexcept;

  PixelIndex x0() const noexcept;
  PixelIndex y0() const noexcept;
  PixelLength width() const noexcept;
  PixelLength height() const noexcept;

  PixelIndex x1() const noexcept;
  PixelIndex y1() const noexcept;
  PixelIndex x_end() const noexcept;
  PixelIndex y_end() const noexcept;

  bool empty() const noexcept;
  void sanitize(PixelLength max_img_width, PixelLength max_img_height) noexcept;

private:
  PixelIndex x0_;
  PixelIndex y0_;
  PixelLength width_;
  PixelLength height_;
};

/** Default constructor. Constructs a bounding box of size (0, 0) top-left corner (0, 0).
 */
inline BoundingBox::BoundingBox() noexcept
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
inline BoundingBox::BoundingBox(PixelIndex x0, PixelIndex y0, PixelLength width, PixelLength height) noexcept
    : x0_(x0), y0_(y0), width_(width), height_(height)
{
}

/** Returns the x-coordinate of the top-left corner, i.e. the x-coordinate of the left box side.
 *
 * \return x-coordinate of the top-left corner.
 */
inline PixelIndex BoundingBox::x0() const noexcept
{
  return x0_;
}

/** Returns the y-coordinate of the top-left corner, i.e. the y-coordinate of the top box side.
 *
 * \return y-coordinate of the top-left corner.
 */
inline PixelIndex BoundingBox::y0() const noexcept
{
  return y0_;
}

/** Returns the width of the bounding box.
 *
 * \return Box width.
 */
inline PixelLength BoundingBox::width() const noexcept
{
  return width_;
}

/** Returns the height of the bounding box.
 *
 * \return Box height.
 */
inline PixelLength BoundingBox::height() const noexcept
{
  return height_;
}

/** Returns the x-coordinate of the bottom-right corner, i.e. the x-coordinate of the right box side.
 *
 * \return x-coordinate of the bottom-right corner.
 */
inline PixelIndex BoundingBox::x1() const noexcept
{
  return PixelIndex(x0_ + width_ - 1);
}

/** Returns the y-coordinate of the bottom-right corner, i.e. the y-coordinate of the bottom box side.
 *
 * \return y-coordinate of the bottom-right corner.
 */
inline PixelIndex BoundingBox::y1() const noexcept
{
  return PixelIndex(y0_ + height_ - 1);
}

/** Returns the x-coordinate one past the the bottom-right corner.
 *
 * \return x-coordinate one past the bottom-right corner.
 */
inline PixelIndex BoundingBox::x_end() const noexcept
{
  return PixelIndex(x0_ + width_);
}

/** Returns the y-coordinate one past the the bottom-right corner.
 *
 * \return y-coordinate one past the bottom-right corner.
 */
inline PixelIndex BoundingBox::y_end() const noexcept
{
  return PixelIndex(y0_ + height_);
}

/** Returns true, if the bounding box has 0 width or height; false otherwise.
 *
 * \return True, if box is empty, false otherwise.
 */
inline bool BoundingBox::empty() const noexcept
{
  return width_ * height_ == 0;
}

/** Sanitizes bounding box w.r.t. given maximum image extents.
 *
 * Modifies bounding box width and height such that the resulting bounding box does not go outside of the maximum
 * image bounds.
 *
 * @param max_img_width Maximum image width.
 * @param max_img_height Maximum image height.
 */
inline void BoundingBox::sanitize(PixelLength max_img_width, PixelLength max_img_height) noexcept
{
  width_ = std::min(width_, PixelLength{max_img_width - x0_});
  height_ = std::min(height_, PixelLength{max_img_height - y0_});
}

}  // namespace sln

#endif  // SELENE_IMG_BOUNDING_BOX_HPP
