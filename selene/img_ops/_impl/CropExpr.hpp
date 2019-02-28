// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_CROP_EXPR_HPP
#define SELENE_IMG_IMPL_CROP_EXPR_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

namespace sln::impl {

template <typename Expr> class CropExpr;

template <typename Expr>
struct ImageExprTraits<CropExpr<Expr>>
    : public ExprTraitsBase
{
  using PixelType = typename Expr::PixelType;
};

template <typename Expr>
class CropExpr : public ImageExpr<CropExpr<Expr>>
{
public:
  using PixelType = typename ImageExprTraits<CropExpr<Expr>>::PixelType;

  explicit CropExpr(const Expr& e, BoundingBox region) : e_(e), region_(region) {}

  TypedLayout layout() const noexcept { return TypedLayout{this->width(), this->height(), this->stride_bytes()}; }

  PixelLength width() const noexcept { return region_.width(); }
  PixelLength height() const noexcept { return region_.height(); }
  Stride stride_bytes() const noexcept { return Stride{PixelTraits<PixelType>::nr_bytes * this->width()}; }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return e_(x + region_.x0(), y + region_.y0());
  }

  template <typename Allocator = default_bytes_allocator>
  decltype(auto) eval() const noexcept
  {
    return Image<PixelType, Allocator>(*this);
  }

private:
  const Expr& e_;
  BoundingBox region_;
};

}  // namespace sln::impl

#endif  // SELENE_IMG_IMPL_CROP_EXPR_HPP
