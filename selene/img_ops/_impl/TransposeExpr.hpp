// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_TRANSPOSE_EXPR_HPP
#define SELENE_IMG_IMPL_TRANSPOSE_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/TransformationDirections.hpp>

namespace sln::impl {

template <bool flip_h, bool flip_v, typename Expr> class TransposeExpr;

template <bool flip_h, bool flip_v, typename Expr>
struct ImageExprTraits<TransposeExpr<flip_h, flip_v, Expr>>
    : public ExprTraitsBase
{
  using PixelType = typename Expr::PixelType;
};

template <bool flip_h, bool flip_v, typename Expr>
class TransposeExpr : public ImageExpr<TransposeExpr<flip_h, flip_v, Expr>>
{
public:
  using PixelType = typename ImageExprTraits<TransposeExpr<flip_h, flip_v, Expr>>::PixelType;

  explicit TransposeExpr(const Expr& e) : e_(e) {}

  TypedLayout layout() const noexcept { return TypedLayout{this->width(), this->height(), this->stride_bytes()}; }

  PixelLength width() const noexcept { return e_.height(); }
  PixelLength height() const noexcept { return e_.width(); }
  Stride stride_bytes() const noexcept { return Stride{PixelTraits<PixelType>::nr_bytes * this->width()}; }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    const auto src_x = flip_v ? PixelIndex{e_.width() - 1 - y} : y;   // branch determined at
    const auto src_y = flip_h ? PixelIndex{e_.height() - 1 - x} : x;  // compile time
    return e_(src_x, src_y);
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

#endif  // SELENE_IMG_IMPL_TRANSPOSE_EXPR_HPP
