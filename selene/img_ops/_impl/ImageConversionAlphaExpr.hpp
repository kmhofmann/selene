// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMPL_IMAGE_CONVERSION_ALPHA_EXPR_HPP
#define SELENE_IMG_IMPL_IMAGE_CONVERSION_ALPHA_EXPR_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/PixelConversions.hpp>

namespace sln::impl {

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename PixelTypeSrc,
    typename PixelTypeDst,
    typename ElementType,
    typename Expr> class ImageConversionAlphaExpr;

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename PixelTypeSrc,
    typename PixelTypeDst,
    typename ElementType,
    typename Expr>
struct ImageExprTraits<ImageConversionAlphaExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, ElementType, Expr>>
    : public ExprTraitsBase
{
  using PixelType = PixelTypeDst;
};

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename PixelTypeSrc,
    typename PixelTypeDst,
    typename ElementType,
    typename Expr>
class ImageConversionAlphaExpr
    : public ImageExpr<ImageConversionAlphaExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, ElementType, Expr>>
{
public:
  using PixelType = typename ImageExprTraits<ImageConversionAlphaExpr<pixel_format_src, pixel_format_dst, PixelTypeSrc, PixelTypeDst, ElementType, Expr>>::PixelType;

  explicit ImageConversionAlphaExpr(const Expr& e, ElementType alpha) : e_(e), alpha_(alpha) {}

  const TypedLayout& layout() const noexcept { return e_.layout(); }

  PixelLength width() const noexcept { return e_.width(); }
  PixelLength height() const noexcept { return e_.height(); }
  Stride stride_bytes() const noexcept { return e_.stride_bytes(); }

  decltype(auto) operator()(PixelIndex x, PixelIndex y) const noexcept
  {
    return PixelConversion<pixel_format_src, pixel_format_dst>::apply(e_(x, y), alpha_);
  }

  template <typename Allocator = default_bytes_allocator>
  decltype(auto) eval() const noexcept
  {
    return Image<PixelType, Allocator>(*this);
  }

private:
  const Expr& e_;
  ElementType alpha_;
};

}  // namespace sln::impl

#endif  // SELENE_IMG_IMPL_IMAGE_CONVERSION_ALPHA_EXPR_HPP
