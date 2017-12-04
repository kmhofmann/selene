// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_BOUNDING_BOX_HPP
#define SELENE_IMG_BOUNDING_BOX_HPP

/// @file

namespace selene {
namespace img {

/** \brief Represents an axis-aligned, rectangular bounding box.
 *
 * Represents a bounding box by its top-left corner and its width and height parameters.
 */
template <typename Index_, typename Length_ = Index_>  // TODO: Remove default for Length_
class BoundingBox
{
public:
  using Index = Index_;  ///< Type representing image coordinates.
  using Length = Length_;  ///< Type representing box extents.

  BoundingBox();
  BoundingBox(Index x0, Index y0, Length width, Length height);

  Index x0() const;
  Index y0() const;
  Length width() const;
  Length height() const;

  Index x1() const;
  Index y1() const;
  Index x_end() const;
  Index y_end() const;

  bool empty() const;

private:
  Index x0_;
  Index y0_;
  Length width_;
  Length height_;
};

/** Default constructor. Constructs a bounding box of size (0, 0) top-left corner (0, 0).
 */
template <typename Index_, typename Length_>
inline BoundingBox<Index_, Length_>::BoundingBox() : x0_(Index{0}), y0_(Index{0}), width_(Length{0}), height_(Length{0})
{
}

/** Constructs a bounding box of size (width, height) with top-left corner (x0, y0).
 *
 * \param x0 x-coordinate of the top-left box corner.
 * \param y0 y-coordinate of the top-left box corner.
 * \param width Box width.
 * \param height Box height.
 */
template <typename Index_, typename Length_>
inline BoundingBox<Index_, Length_>::BoundingBox(Index x0, Index y0, Length width, Length height)
    : x0_(x0), y0_(y0), width_(width), height_(height)
{
}

/** Returns the x-coordinate of the top-left corner, i.e. the x-coordinate of the left box side.
 *
 * \return x-coordinate of the top-left corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x0() const
{
  return x0_;
}

/** Returns the y-coordinate of the top-left corner, i.e. the y-coordinate of the top box side.
 *
 * \return y-coordinate of the top-left corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y0() const
{
  return y0_;
}

/** Returns the width of the bounding box.
 *
 * \return Box width.
 */
template <typename Index_, typename Length_>
inline Length_ BoundingBox<Index_, Length_>::width() const
{
  return width_;
}

/** Returns the height of the bounding box.
 *
 * \return Box height.
 */
template <typename Index_, typename Length_>
inline Length_ BoundingBox<Index_, Length_>::height() const
{
  return height_;
}

/** Returns true, if the bounding box has 0 width or height; false otherwise.
 *
 * \return True, if box is empty, false otherwise.
 */
template <typename Index_, typename Length_>
inline bool BoundingBox<Index_, Length_>::empty() const
{
  return width_ * height_ == 0;
}

/** Returns the x-coordinate of the bottom-right corner, i.e. the x-coordinate of the right box side.
 *
 * \return x-coordinate of the bottom-right corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x1() const
{
  return x0_ + width_ - 1;
}

/** Returns the y-coordinate of the bottom-right corner, i.e. the y-coordinate of the bottom box side.
 *
 * \return y-coordinate of the bottom-right corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y1() const
{
  return y0_ + height_ - 1;
}

/** Returns the x-coordinate one past the the bottom-right corner.
 *
 * \return x-coordinate one past the bottom-right corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x_end() const
{
  return x0_ + width_;
}

/** Returns the y-coordinate one past the the bottom-right corner.
 *
 * \return y-coordinate one past the bottom-right corner.
 */
template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y_end() const
{
  return y0_ + height_;
}

}  // namespace img
}  // namespace selene

#endif  // SELENE_IMG_BOUNDING_BOX_HPP
