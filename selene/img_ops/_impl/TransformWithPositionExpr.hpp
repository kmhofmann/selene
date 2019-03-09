// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_TRANSFORM_WITH_POSITION_EXPR_HPP
#define SELENE_IMG_IMPL_TRANSFORM_WITH_POSITION_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <type_traits>

namespace sln::impl {

template <typename Expr, typename Function> class TransformWithPositionExpr;

template <typename Expr, typename Function>
struct ImageExprTraits<TransformWithPositionExpr<Expr, Function>>
    : public ExprTraitsBase
{
  using PixelType = std::invoke_result_t<Function, typename Expr::PixelType, PixelIndex, PixelIndex>;
};

template <typename Expr, typename Function>
class TransformWithPositionExpr : public ImageExpr<TransformWithPositionExpr<Expr, Function>>
{
public:
  using PixelType = typename ImageExprTraits<TransformWithPositionExpr<Expr, Function>>::PixelType;

  explicit TransformWithPositionExpr(const Expr& e, const Function& func) : e_(e), func_(func) {}

  const TypedLayout& layout() const noexcept { return e_.layout(); }

  PixelLength width() const noexcept { return e_.width(); }
  PixelLength height() const noexcept { return e_.height(); }
  Stride stride_bytes() const noexcept { return e_.stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return func_(e_(x, y), x, y);
  }

  template <typename Allocator = default_bytes_allocator>
  decltype(auto) eval() const noexcept
  {
    return Image<PixelType, Allocator>(*this);
  }

private:
  const Expr& e_;
  const Function& func_;
};

}  // namespace sln::impl

#endif  // SELENE_IMG_IMPL_TRANSFORM_WITH_POSITION_EXPR_HPP
