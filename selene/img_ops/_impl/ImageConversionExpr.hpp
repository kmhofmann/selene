// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_IMAGE_CONVERSION_EXPR_HPP
#define SELENE_IMG_IMPL_IMAGE_CONVERSION_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/PixelConversions.hpp>

namespace sln::impl {

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelTypeSrc,
          typename PixelTypeDst,
          typename Expr> class ImageConversionExpr;

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelTypeSrc,
          typename PixelTypeDst,
          typename Expr>
struct ImageExprTraits<ImageConversionExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, Expr>>
    : public ExprTraitsBase
{
  using PixelType = PixelTypeDst;
};

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename PixelTypeSrc,
    typename PixelTypeDst,
    typename Expr>
class ImageConversionExpr
    : public ImageExpr<ImageConversionExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, Expr>>
{
public:
  using PixelType = typename ImageExprTraits<ImageConversionExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, Expr>>::PixelType;

  explicit ImageConversionExpr(const Expr& e) : e_(e) {}

  TypedLayout layout() const noexcept { return e_.layout(); }

  PixelLength width() const noexcept { return e_.width(); }
  PixelLength height() const noexcept { return e_.height(); }
  Stride stride_bytes() const noexcept { return e_.stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return PixelConversion<pixel_format_src, pixel_format_dst>::apply(e_(x, y));
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

#endif  // SELENE_IMG_IMPL_IMAGE_CONVERSION_EXPR_HPP
