// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_IDENTITY_EXPR_HPP
#define SELENE_IMG_IMPL_IDENTITY_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

namespace sln::impl {

template <typename Expr> class IdentityExpr;

template <typename Expr>
struct ImageExprTraits<IdentityExpr<Expr>>
    : public ExprTraitsBase
{
  using PixelType = typename Expr::PixelType;
};

template <typename Expr>
class IdentityExpr : public ImageExpr<IdentityExpr<Expr>>
{
public:
  using PixelType = typename ImageExprTraits<IdentityExpr<Expr>>::PixelType;

  explicit IdentityExpr(const Expr& e) : e_(e) {}

  TypedLayout layout() const noexcept { return e_.layout(); }

  PixelLength width() const noexcept { return e_.width(); }
  PixelLength height() const noexcept { return e_.height(); }
  Stride stride_bytes() const noexcept { return e_.stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return e_(x, y);
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

#endif  // SELENE_IMG_IMPL_IDENTITY_EXPR_HPP
