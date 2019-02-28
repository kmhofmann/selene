// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_FLIP_EXPR_HPP
#define SELENE_IMG_IMPL_FLIP_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/TransformationDirections.hpp>

namespace sln::impl {

template <FlipDirection flip_dir, typename Expr> class FlipExpr;

template <FlipDirection flip_dir, typename Expr>
struct ImageExprTraits<FlipExpr<flip_dir, Expr>>
    : public ExprTraitsBase
{
  using PixelType = typename Expr::PixelType;
};

template <FlipDirection flip_dir, typename Expr>
class FlipExpr : public ImageExpr<FlipExpr<flip_dir, Expr>>
{
public:
  using PixelType = typename ImageExprTraits<FlipExpr<flip_dir, Expr>>::PixelType;

  explicit FlipExpr(const Expr& e) : e_(e) {}

  const TypedLayout& layout() const noexcept { return e_.layout(); }

  PixelLength width() const noexcept { return e_.width(); }
  PixelLength height() const noexcept { return e_.height(); }
  Stride stride_bytes() const noexcept { return e_.stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    if constexpr (flip_dir == FlipDirection::Horizontal)
    {
      return e_(PixelIndex{e_.width() - 1 - x}, y);
    }
    else if constexpr (flip_dir == FlipDirection::Vertical)
    {
      return e_(x, PixelIndex{e_.height() - 1 - y});
    }
    else
    {
      return e_(PixelIndex{e_.width() - 1 - x}, PixelIndex{e_.height() - 1 - y});
    }
  }

  template <typename Allocator = default_bytes_allocator>
  decltype(auto) eval() const noexcept
  {
    return Image<PixelType, Allocator>(*this);
  }

private:
  const Expr& e_;
};

}  // namespace sln::impl

#endif  // SELENE_IMG_IMPL_FLIP_EXPR_HPP
