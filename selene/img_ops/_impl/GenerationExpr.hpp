// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_GENERATION_EXPR_HPP
#define SELENE_IMG_IMPL_GENERATION_EXPR_HPP

/// @file

#include <selene/base/Types.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <type_traits>

namespace sln::impl {

template <typename Expr> class GenerationExpr;

template <typename Function>
struct ImageExprTraits<GenerationExpr<Function>>
    : public ExprTraitsBase
{
  using PixelType = std::invoke_result_t<Function, PixelIndex, PixelIndex>;
};

template <typename Function>
class GenerationExpr : public ImageExpr<GenerationExpr<Function>>
{
public:
  using PixelType = typename ImageExprTraits<GenerationExpr<Function>>::PixelType;

  explicit GenerationExpr(const Function& func,
                          PixelLength width,
                          PixelLength height)
      : func_(func), width_(width), height_(height)
  { }

  TypedLayout layout() const noexcept { return TypedLayout{this->width(), this->height(), this->stride_bytes()}; }

  PixelLength width() const noexcept { return width_; }
  PixelLength height() const noexcept { return height_; }
  Stride stride_bytes() const noexcept { return Stride{PixelTraits<PixelType>::nr_bytes * this->width()}; }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return func_(x, y);
  }

  template <typename Allocator = default_bytes_allocator>
  decltype(auto) eval() const noexcept
  {
    return Image<PixelType, Allocator>(*this);
  }

private:
  Function func_;
  PixelLength width_;
  PixelLength height_;
};

}  // namespace sln::impl

#endif  // SELENE_IMG_IMPL_GENERATION_EXPR_HPP
