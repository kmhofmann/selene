// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TRANSFORMATIONS_HPP
#define SELENE_IMG_TRANSFORMATIONS_HPP

/// @file

#include <selene/base/Assert.hpp>

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/Allocate.hpp>
#include <selene/img_ops/TransformationDirections.hpp>
#include <selene/img_ops/_impl/FlipExpr.hpp>
#include <selene/img_ops/_impl/IdentityExpr.hpp>
#include <selene/img_ops/_impl/TransposeExpr.hpp>

#include <algorithm>
#include <utility>


namespace sln {

/// \addtogroup group-img-ops
/// @{

// Flip:

template <FlipDirection flip_dir, typename DerivedSrcDst>
void flip(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst);

template <FlipDirection flip_dir, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> flip(const ImageBase<DerivedSrc>& img);

template <FlipDirection flip_dir, typename DerivedSrc>
auto flip_expr(const ImageExpr<DerivedSrc>& img);

template <typename DerivedSrcDst>
void flip_horizontally_in_place(ImageBase<DerivedSrcDst>& img);

template <typename DerivedSrcDst>
void flip_vertically_in_place(ImageBase<DerivedSrcDst>& img);

// Transpose:

template <bool flip_h = false, bool flip_v = false, typename DerivedSrcDst>
void transpose(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst);

template <bool flip_h = false, bool flip_v = false, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> transpose(const ImageBase<DerivedSrc>& img);

template <bool flip_h = false, bool flip_v = false, typename DerivedSrc>
auto transpose_expr(const ImageExpr<DerivedSrc>& img);

// Rotate:

template <RotationDirection rot_dir, typename DerivedSrcDst>
void rotate(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst);

template <RotationDirection rot_dir, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> rotate(const ImageBase<DerivedSrc>& img);

template <RotationDirection rot_dir, typename DerivedSrc>
auto rotate_expr(const ImageExpr<DerivedSrc>& img);

/// @}

// ----------
// Implementation:

/** \brief Flip the image contents according to the specified flip direction.
 *
 * @tparam flip_dir The flip direction. Must be provided
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img_src The source image.
 * @param[out] img_dst The flipped output image.
 */
template <FlipDirection flip_dir, typename DerivedSrcDst>
void flip(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst)
{
  SELENE_ASSERT(&img_src != &img_dst);
  allocate(img_dst, img_src.layout());

  switch (flip_dir)
  {
    case FlipDirection::Horizontal:
    {
      for (auto y_src = 0_idx; y_src < img_src.height(); ++y_src)
      {
        std::reverse_copy(img_src.data(y_src), img_src.data_row_end(y_src), img_dst.data(y_src));
      }
      break;
    }
    case FlipDirection::Vertical:
    {
      for (auto y_src = 0_idx; y_src < img_src.height(); ++y_src)
      {
        const auto y_dst = PixelIndex{img_src.height() - y_src - 1};
        std::copy(img_src.data(y_src), img_src.data_row_end(y_src), img_dst.data(y_dst));
      }
      break;
    }
    case FlipDirection::Both:
    {
      for (auto y_src = 0_idx; y_src < img_src.height(); ++y_src)
      {
        const auto y_dst = PixelIndex{img_src.height() - y_src - 1};
        std::reverse_copy(img_src.data(y_src), img_src.data_row_end(y_src), img_dst.data(y_dst));
      }
      break;
    }
  }
}

/** \brief Flip the image contents according to the specified flip direction.
 *
 * @tparam flip_dir The flip direction. Must be provided
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return The flipped output image.
 */
template <FlipDirection flip_dir, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> flip(const ImageBase<DerivedSrc>& img)
{
  Image<typename DerivedSrc::PixelType> img_flip;
  flip<flip_dir>(img, img_flip);
  return img_flip;
}

/** \brief Flip the image contents according to the specified flip direction.
 *
 * This function returns an expression that is convertible to the flipped image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * @tparam flip_dir The flip direction. Must be provided
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return An expression that is convertible to the flipped output image.
 */
template <FlipDirection flip_dir, typename DerivedSrc>
auto flip_expr(const ImageExpr<DerivedSrc>& img)
{
  return impl::FlipExpr<flip_dir, ImageExpr<DerivedSrc>>(img);
}

/** \brief Flip the image horizontally, in-place.
 *
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img The image to be flipped horizontally.
 */
template <typename DerivedSrcDst>
void flip_horizontally_in_place(ImageBase<DerivedSrcDst>& img)
{
  const auto half_width = img.width() / 2;

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    auto row_ptr = img.data(y);

    for (auto x_left = 0_idx; x_left < half_width; ++x_left)
    {
      using std::swap;
      const auto x_right = img.width() - x_left - 1;
      swap(row_ptr[ptrdiff_t{x_left}], row_ptr[ptrdiff_t{x_right}]);
    }
  }
}

/** \brief Flip the image vertically, in-place.
 *
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img The image to be flipped vertically.
 */
template <typename DerivedSrcDst>
void flip_vertically_in_place(ImageBase<DerivedSrcDst>& img)
{
  const auto half_height = img.height() / 2;

  for (auto y_top = 0_idx; y_top < half_height; ++y_top)
  {
    const auto y_bottom = PixelIndex{img.height() - y_top - 1};

    const auto x_top_end = img.data_row_end(y_top);
    auto x_top = img.data(y_top);
    auto x_bottom = img.data(y_bottom);

    for (; x_top != x_top_end; ++x_top, ++x_bottom)
    {
      using std::swap;
      swap(*x_top, *x_bottom);
    }
  }

  // TODO: measure
#if 0
  std::vector<PixelType> buffer(old_img.width());  // TODO: could be stack-based in some cases?

  for (auto y_top = 0_px; y_top < half_height; ++y_top)
  {
    const auto y_bottom = PixelIndex{old_img.height() - 1 - y_top};
    std::copy(old_img.data(y_top), old_img.data_row_end(y_top), buffer.begin());
    std::copy(old_img.data(y_bottom), old_img.data_row_end(y_bottom), old_img.data(y_top));
    std::copy(std::cbegin(buffer), std::cend(buffer), old_img.data(y_bottom));
  }
#endif
}

/** \brief Transpose the image.
 *
 * The output image will have transposed extents, i.e. output width will be input height, and output height will be
 * input width.
 *
 * @tparam flip_h If true, the output will additionally be horizontally flipped.
 * @tparam flip_v If true, the output will additionally be vertically flipped.
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img_src The source image.
 * @param[out] img_dst The transposed output image.
 */
template <bool flip_h, bool flip_v, typename DerivedSrcDst>
void transpose(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst)
{
  SELENE_ASSERT(&img_src != &img_dst);
  allocate(img_dst, {img_src.height(), img_src.width()});

   for (auto dst_y = 0_idx; dst_y < img_dst.height(); ++dst_y)
  {
    for (auto dst_x = 0_idx; dst_x < img_dst.width(); ++dst_x)
    {
      const auto src_x = flip_v ? PixelIndex{img_src.width() - 1 - dst_y} : dst_y;  // branch determined at
      const auto src_y = flip_h ? PixelIndex{img_src.height() - 1 - dst_x} : dst_x;  // compile time
      img_dst(dst_x, dst_y) = img_src(src_x, src_y);
    }
  }
}

/** \brief Transpose the image.
 *
 * The output image will have transposed extents, i.e. output width will be input height, and output height will be
 * input width.
 *
 * @tparam flip_h If true, the output will additionally be horizontally flipped.
 * @tparam flip_v If true, the output will additionally be vertically flipped.
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return The transposed output image.
 */
template <bool flip_h, bool flip_v, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> transpose(const ImageBase<DerivedSrc>& img)
{
  Image<typename DerivedSrc::PixelType> img_t;
  transpose(img, img_t);
  return img_t;
}

/** \brief Transpose the image.
 *
 * This function returns an expression that is convertible to the transposed image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * The output image will have transposed extents, i.e. output width will be input height, and output height will be
 * input width.
 *
 * @tparam flip_h If true, the output will additionally be horizontally flipped.
 * @tparam flip_v If true, the output will additionally be vertically flipped.
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return An expression that is convertible to the transposed output image.
 */
template <bool flip_h, bool flip_v, typename DerivedSrc>
auto transpose_expr(const ImageExpr<DerivedSrc>& img)
{
  return impl::TransposeExpr<flip_h, flip_v, ImageExpr<DerivedSrc>>(img);
}

/** \brief Rotate the image (in 90 degree increments) by the specified amount and direction.
 *
 * @tparam rot_dir The rotation amount and direction. Must be provided.
 * @tparam DerivedSrcDst The typed source/target image type.
 * @param img_src The source image.
 * @param[out] img_dst The rotated output image.
 *
 */
template <RotationDirection rot_dir, typename DerivedSrcDst>
void rotate(const ImageBase<DerivedSrcDst>& img_src, ImageBase<DerivedSrcDst>& img_dst)
{
  SELENE_ASSERT(&img_src != &img_dst);

  if constexpr (rot_dir == RotationDirection::Clockwise0 || rot_dir == RotationDirection::Counterclockwise0)
  {
    clone(img_src, img_dst);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise90 || rot_dir == RotationDirection::Counterclockwise270)
  {
    transpose<true, false>(img_src, img_dst);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise180 || rot_dir == RotationDirection::Counterclockwise180)
  {
    flip<FlipDirection::Both>(img_src, img_dst);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise270 || rot_dir == RotationDirection::Counterclockwise90)
  {
    transpose<false, true>(img_src, img_dst);
  }
}

/** \brief Rotate the image (in 90 degree increments) by the specified amount and direction.
 *
 * @tparam rot_dir The rotation amount and direction. Must be provided.
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return The rotated output image.
 */
template <RotationDirection rot_dir, typename DerivedSrc>
Image<typename DerivedSrc::PixelType> rotate(const ImageBase<DerivedSrc>& img)
{
  Image<typename DerivedSrc::PixelType> img_r;
  rotate<rot_dir>(img, img_r);
  return img_r;
}

/** \brief Rotate the image (in 90 degree increments) by the specified amount and direction.
 *
 * This function returns an expression that is convertible to the transposed image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * @tparam rot_dir The rotation amount and direction. Must be provided.
 * @tparam DerivedSrc The typed source image type.
 * @param img The source image.
 * @return An expression that is convertible to the rotated output image.
 */
template <RotationDirection rot_dir, typename DerivedSrc>
auto rotate_expr(const ImageExpr<DerivedSrc>& img)
{
  if constexpr (rot_dir == RotationDirection::Clockwise0 || rot_dir == RotationDirection::Counterclockwise0)
  {
    return impl::IdentityExpr<ImageExpr<DerivedSrc>>(img);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise90 || rot_dir == RotationDirection::Counterclockwise270)
  {
    return impl::TransposeExpr<true, false, ImageExpr<DerivedSrc>>(img);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise180 || rot_dir == RotationDirection::Counterclockwise180)
  {
    return impl::FlipExpr<FlipDirection::Both, ImageExpr<DerivedSrc>>(img);
  }
  else if constexpr (rot_dir == RotationDirection::Clockwise270 || rot_dir == RotationDirection::Counterclockwise90)
  {
    return impl::TransposeExpr<false, true, ImageExpr<DerivedSrc>>(img);
  }
}

}  // namespace sln

#endif  // SELENE_IMG_TRANSFORMATIONS_HPP
