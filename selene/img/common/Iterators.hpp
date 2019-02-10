// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_ITERATORS_HPP
#define SELENE_IMG_TYPED_IMAGE_ITERATORS_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>
#include <selene/img/common/Types.hpp>

#include <type_traits>

namespace sln {

/// \addtogroup group-img-typed
/// @{

template <typename View, typename Row>
class ImageRowIterator;

/** \brief Represents an image row whose elements can be iterated.
 */
template <typename View_, bool is_const_>
class ImageRow
{
public:
  using View = View_;
  using PixelType = typename View::PixelType;
  constexpr static bool is_const = is_const_;

  using iterator = PixelType*;  ///< The iterator type.
  using const_iterator = const PixelType*;  ///< The const_iterator type.

  ~ImageRow() = default;  ///< Destructor.
  ImageRow(const ImageRow<View_, is_const_>&) = default;  ///< Copy constructor.
  ImageRow& operator=(const ImageRow<View_, is_const_>&) = default;  ///< Copy assigment operator.
  ImageRow(ImageRow<View_, is_const_>&&) noexcept = default;  ///< Copy constructor.
  ImageRow& operator=(ImageRow<View_, is_const_>&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns an iterator to the first element of the image row.
   *
   * @return Iterator to the first row element.
   */
  iterator begin() noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const_iterator begin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const_iterator cbegin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns an iterator to the one-past-the-last element of the image row.
   *
   * @return Iterator to the one-past-the-last row element.
   */
  iterator end() noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const_iterator end() const noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const_iterator cend() const noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns the row index.
   *
   * @return Row index.
   */
  PixelIndex index() const noexcept
  {
    return row_index_;
  }

  /** \brief Indicates row equality.
   *
   * @param it Another image row.
   * @return True, if both instances refer to the same image row; false otherwise.
   */
  template <bool is_const_2> bool operator==(const ImageRow<View, is_const_2>& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  template <bool is_const_2> bool operator<(const ImageRow<View, is_const_2>& it) const noexcept
  {
    return img_ == it.img_ && row_index_ < it.row_index_;
  }

  template <bool is_const_2> bool operator>(const ImageRow<View, is_const_2>& it) const noexcept
  {
    return img_ == it.img_ && row_index_ > it.row_index_;
  }

  operator ImageRow<View, true>() const
  {
    return ImageRow<View, true>(img_, row_index_);
  }

private:
  using ViewPtr = std::conditional_t<is_const, const View*, View*>;
  ViewPtr img_;
  PixelIndex row_index_;

  ImageRow(ViewPtr img, PixelIndex row_index) : img_(img), row_index_(row_index)
  { }

  friend View;
  friend class ImageRow<View, !is_const>;
  friend class ImageRowIterator<View, ImageRow<View, is_const>>;
};


/** \brief Represents an image row whose elements can be iterated.
 */
template <typename View_, typename PixelType_, bool is_const_>
class DynImageRow
{
public:
  using View = View_;
  using PixelType = PixelType_;
  constexpr static bool is_const = is_const_;

  DynImageRow(View img, PixelIndex row_index) : img_(img), row_index_(row_index)
  { }

  ~DynImageRow() = default;  ///< Destructor.
  DynImageRow(const DynImageRow<View_, PixelType_, is_const_>&) = default;  ///< Copy constructor.
  DynImageRow& operator=(const DynImageRow<View_, PixelType_, is_const_>&) = default;  ///< Copy assigment operator.
  DynImageRow(DynImageRow<View_, PixelType_, is_const_>&&) noexcept = default;  ///< Copy constructor.
  DynImageRow& operator=(DynImageRow<View_, PixelType_, is_const_>&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns an iterator to the first element of the image row.
   *
   * @return Iterator to the first row element.
   */
  PixelType* begin() noexcept
  {
    return img_.data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* begin() const noexcept
  {
    return img_.data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* cbegin() const noexcept
  {
    return img_.data(row_index_);
  }

  /** \brief Returns an iterator to the one-past-the-last element of the image row.
   *
   * @return Iterator to the one-past-the-last row element.
   */
  PixelType* end() noexcept
  {
    return img_.data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* end() const noexcept
  {
    return img_.data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* cend() const noexcept
  {
    return img_.data_row_end(row_index_);
  }

  /** \brief Returns the row index.
   *
   * @return Row index.
   */
  PixelIndex index() const noexcept
  {
    return row_index_;
  }

  /** \brief Indicates row equality.
   *
   * @param it Another image row.
   * @return True, if both instances refer to the same image row; false otherwise.
   */
  template <typename View2, bool is_const_2>
  bool operator==(const DynImageRow<View2, PixelType, is_const_2>& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  template <typename View2, bool is_const_2>
  bool operator<(const DynImageRow<View2, PixelType, is_const_2>& it) const noexcept
  {
    return img_ == it.img_ && row_index_ < it.row_index_;
  }

  template <typename View2, bool is_const_2>
  bool operator>(const DynImageRow<View2, PixelType, is_const_2>& it) const noexcept
  {
    return img_ == it.img_ && row_index_ > it.row_index_;
  }

  operator DynImageRow<View, PixelType, true>() const
  {
    return DynImageRow<View, PixelType, true>(img_, row_index_);
  }

private:
  View img_;
  PixelIndex row_index_;

  template <typename View2, typename PixelType2, bool is_const_2> friend class DynImageRow;
  friend class ImageRowIterator<View, DynImageRow<View, PixelType, is_const>>;
};



/** \brief Random access iterator over rows of an image.
 */
template <typename View, typename Row>
class ImageRowIterator
{
public:
  using difference_type = std::ptrdiff_t;
  using value_type = Row;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;

  explicit ImageRowIterator(Row row) : row_(row)
  {
  }

  ~ImageRowIterator() = default;  ///< Destructor.
  ImageRowIterator(const ImageRowIterator<View, Row>&) = default;  ///< Copy constructor.
  ImageRowIterator& operator=(const ImageRowIterator<View, Row>&) = default;  ///< Copy assignment operator.
  ImageRowIterator(ImageRowIterator<View, Row>&&) noexcept = default;  ///< Move constructor.
  ImageRowIterator& operator=(ImageRowIterator<View, Row>&&) noexcept = default;  ///< Move assignment operator.

  // Increment and decrement

  ImageRowIterator<View, Row>& operator--() noexcept
  {
    --row_.row_index_;
    return *this;
  }

  ImageRowIterator<View, Row> operator--(int) noexcept
  {
    ImageRowIterator it(*this);
    operator--();
    return it;
  }

  ImageRowIterator<View, Row>& operator++() noexcept
  {
    ++row_.row_index_;
    return *this;
  }

  ImageRowIterator<View, Row> operator++(int) noexcept
  {
    ImageRowIterator it(*this);
    operator++();
    return it;
  }

  ImageRowIterator<View, Row>& operator-=(difference_type n) noexcept
  {
    row_.row_index_ -= n;
    return *this;
  }

  ImageRowIterator<View, Row>& operator+=(difference_type n) noexcept
  {
    row_.row_index_ += n;
    return *this;
  }

  // Iterator +/- value

  friend ImageRowIterator<View, Row> operator-(const ImageRowIterator<View, Row>& it, difference_type n) noexcept
  {
    return ImageRowIterator<View, Row>(
        Row(it.row_.img_, it.row_.row_index_ - n));
  }

  friend ImageRowIterator<View, Row> operator+(const ImageRowIterator<View, Row>& it, difference_type n) noexcept
  {
    return ImageRowIterator<View, Row>(
        Row(it.row_.img_, it.row_.row_index_ + n));
  }

  friend ImageRowIterator<View, Row> operator+(difference_type n, const ImageRowIterator<View, Row>& it) noexcept
  {
    return ImageRowIterator<View, Row>(
        Row(it.row_.img_, it.row_.row_index_ + n));
  }

  // Iterator - iterator

  friend difference_type operator-(const ImageRowIterator<View, Row>& it_l,
                                   const ImageRowIterator<View, Row>& it_r) noexcept
  {
    SELENE_ASSERT(it_l.row_.img_ == it_r.row_.img_);
    return it_l.row_.row_index_ - it_r.row_.row_index_;
  }

  // Equality and ordering

  /** \brief Indicates iterator equality.
   *
   * @param it Another ImageRowIterator instance.
   * @return True, if both iterators reference the same row; false otherwise.
   */
  template <typename View2, typename Row2>
  bool operator==(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return row_ == it.row_;
  }

  /** \brief Indicates iterator inequality.
   *
   * @param it Another ImageRowIterator instance.
   * @return True, if both iterators not reference the same row; false if they do.
   */
  template <typename View2, typename Row2>
  bool operator!=(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return !(row_ == it.row_);
  }

  template <typename View2, typename Row2>
  bool operator<(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return row_ < it.row_;
  }

  template <typename View2, typename Row2>
  bool operator>(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return row_ > it.row_;
  }

  template <typename View2, typename Row2>
  bool operator<=(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return !(row_ > it.row_);
  }

  template <typename View2, typename Row2>
  bool operator>=(const ImageRowIterator<View2, Row2>& it) const noexcept
  {
    return !(row_ < it.row_);
  }

  /** Iterator dereferencing operator.
   *
   * @return An instance representing the respective row.
   */
  Row& operator*() noexcept
  {
    return row_;
  }

  /** Iterator dereferencing operator.
   *
   * @return An instance representing the respective row.
   */
  const Row& operator*() const noexcept
  {
    return row_;
  }

  /** Support conversion from iterator to const_iterator. */
  template <typename View2, typename Row2, typename = std::enable_if_t<Row2::is_const>>
  operator ImageRowIterator<View2, Row2>() const
  {
    static_assert(std::is_same_v<typename Row::View, typename Row2::View>);
    return ImageRowIterator<View2, Row2>(row_);
  }

private:
  Row row_;

  template <typename View2, typename Row2> friend class ImageRowIterator;
};

/// @}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_ITERATORS_HPP
