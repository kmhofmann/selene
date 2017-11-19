// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_BOUNDING_BOX_HPP
#define SELENE_IMG_BOUNDING_BOX_HPP

namespace selene {
namespace img {

// TODO: Remove default for Length_
template <typename Index_, typename Length_ = Index_>
class BoundingBox
{
public:
  using Index = Index_;
  using Length = Length_;

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

template <typename Index_, typename Length_>
inline BoundingBox<Index_, Length_>::BoundingBox()
    : x0_(Index{0}), y0_(Index{0}), width_(Length{0}), height_(Length{0})
{
}

template <typename Index_, typename Length_>
inline BoundingBox<Index_, Length_>::BoundingBox(Index x0, Index y0, Length width, Length height)
    : x0_(x0), y0_(y0), width_(width), height_(height)
{
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x0() const
{
  return x0_;
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y0() const
{
  return y0_;
}

template <typename Index_, typename Length_>
inline Length_ BoundingBox<Index_, Length_>::width() const
{
  return width_;
}

template <typename Index_, typename Length_>
inline Length_ BoundingBox<Index_, Length_>::height() const
{
  return height_;
}

template <typename Index_, typename Length_>
inline bool BoundingBox<Index_, Length_>::empty() const
{
  return width_ * height_ == 0;
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x1() const
{
  return x0_ + width_ - 1;
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y1() const
{
  return y0_ + height_ - 1;
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::x_end() const
{
  return x0_ + width_;
}

template <typename Index_, typename Length_>
inline Index_ BoundingBox<Index_, Length_>::y_end() const
{
  return y0_ + height_;
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_BOUNDING_BOX_HPP