// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_DYN_IMAGE_ITERATORS_HPP
#define SELENE_IMG_DYNAMIC_DYN_IMAGE_ITERATORS_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

/// \addtogroup group-img-dynamic
/// @{

template <ImageModifiability modifiability>
class DynImageView;

template <typename PixelType, ImageModifiability modifiability>
class DynImageRowIterator;

template <typename PixelType, ImageModifiability modifiability>
class ConstDynImageRowIterator;

/** \brief Represents an image row (of a non-const image) whose elements can be iterated through.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType, ImageModifiability modifiability>
class DynImageRow
{
public:
  ~DynImageRow() = default;  ///< Destructor.
  DynImageRow(const DynImageRow<PixelType, modifiability>&) = default;  ///< Copy constructor.
  DynImageRow& operator=(const DynImageRow<PixelType, modifiability>&) = default;  ///< Copy assigment operator.
  DynImageRow(DynImageRow<PixelType, modifiability>&&) noexcept = default;  ///< Copy constructor.
  DynImageRow& operator=(DynImageRow<PixelType, modifiability>&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns an iterator to the first element of the image row.
   *
   * @return Iterator to the first row element.
   */
  PixelType* begin() noexcept
  {
    return img_->template data<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* begin() const noexcept
  {
    return img_->template data<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* cbegin() const noexcept
  {
    return img_->template data<PixelType>(row_index_);
  }

  /** \brief Returns an iterator to the one-past-the-last element of the image row.
   *
   * @return Iterator to the one-past-the-last row element.
   */
  PixelType* end() noexcept
  {
    return img_->template data_row_end<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* end() const noexcept
  {
    return img_->template data_row_end<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* cend() const noexcept
  {
    return img_->template data_row_end<PixelType>(row_index_);
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
  bool operator==(const DynImageRow& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  /** \brief Indicates row inequality.
   *
   * @param it Another image row.
   * @return True, if both instances do not refer to the same image row; false if they do.
   */
  bool operator!=(const DynImageRow& it) const noexcept
  {
    return row_index_ != it.row_index_ || img_ != it.img_;
  }

private:
  DynImageView<modifiability>* img_;
  PixelIndex row_index_;

  DynImageRow(DynImageView<modifiability>* img, PixelIndex row_index) : img_(img), row_index_(row_index)
  {
  }

  friend class DynImageView<modifiability>;
  friend class DynImageRowIterator<PixelType, modifiability>;
};


/** \brief Bidirectional iterator over rows of a (non-const) image.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType, ImageModifiability modifiability>
class DynImageRowIterator
{
public:
  ~DynImageRowIterator() = default;  ///< Destructor.
  DynImageRowIterator(const DynImageRowIterator<PixelType, modifiability>&) = default;  ///< Copy constructor.
  DynImageRowIterator& operator=(const DynImageRowIterator<PixelType, modifiability>&) = default;  ///< Copy assignment operator.
  DynImageRowIterator(DynImageRowIterator<PixelType, modifiability>&&) noexcept = default;  ///< Move constructor.
  DynImageRowIterator& operator=(DynImageRowIterator<PixelType, modifiability>&&) noexcept = default;  ///< Move assignment operator.

  /** \brief Iterator pre-decrement.
   *
   * @return Reference to *this.
   */
  DynImageRowIterator<PixelType, modifiability>& operator--() noexcept
  {
    --row_.row_index_;
    return *this;
  }

  /** \brief Iterator pre-increment.
   *
   * @return Reference to *this.
   */

  DynImageRowIterator<PixelType, modifiability>& operator++() noexcept
  {
    ++row_.row_index_;
    return *this;
  }

  /** \brief Iterator post-decrement.
   *
   * @return Reference to *this.
   */
  const DynImageRowIterator<PixelType, modifiability> operator--(int) noexcept
  {
    DynImageRowIterator it(*this);
    operator--();
    return it;
  }

  /** \brief Iterator post-increment.
   *
   * @return Reference to *this.
   */
  const DynImageRowIterator<PixelType, modifiability> operator++(int) noexcept
  {
    DynImageRowIterator it(*this);
    operator++();
    return it;
  }

  /** \brief Indicates iterator equality.
   *
   * @param it Another DynImageRowIterator instance.
   * @return True, if both iterators reference the same row; false otherwise.
   */
  bool operator==(const DynImageRowIterator<PixelType, modifiability>& it) const noexcept
  {
    return row_ == it.row_;
  }

  /** \brief Indicates iterator inequality.
   *
   * @param it Another DynImageRowIterator instance.
   * @return True, if both iterators not reference the same row; false if they do.
   */
  bool operator!=(const DynImageRowIterator<PixelType, modifiability>& it) const noexcept
  {
    return row_ != it.row_;
  }

  /** Iterator dereferencing operator.
   *
   * @return An `DynImageRow` instance representing the respective row.
   */
  DynImageRow<PixelType, modifiability>& operator*() noexcept
  {
    return row_;
  }

private:
  DynImageRow<PixelType, modifiability> row_;

  explicit DynImageRowIterator(DynImageRow<PixelType, modifiability> row) : row_(row)
  {
  }

  friend class DynImageView<modifiability>;
};


/** \brief Represents an image row (of a const image) whose elements can be iterated through.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType, ImageModifiability modifiability>
class ConstDynImageRow
{
public:
  ~ConstDynImageRow() = default;  ///< Destructor.
  ConstDynImageRow(const ConstDynImageRow<PixelType, modifiability>&) = default;  ///< Copy constructor.
  ConstDynImageRow& operator=(const ConstDynImageRow<PixelType, modifiability>&) = default;  ///< Copy assigment operator.
  ConstDynImageRow(ConstDynImageRow<PixelType, modifiability>&&) noexcept = default;  ///< Copy constructor.
  ConstDynImageRow& operator=(ConstDynImageRow<PixelType, modifiability>&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* begin() const noexcept
  {
    return img_->template data<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* cbegin() const noexcept
  {
    return img_->template data<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* end() const noexcept
  {
    return img_->template data_row_end<PixelType>(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* cend() const noexcept
  {
    return img_->template data_row_end<PixelType>(row_index_);
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
  bool operator==(const ConstDynImageRow& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  /** \brief Indicates row inequality.
   *
   * @param it Another image row.
   * @return True, if both instances do not refer to the same image row; false if they do.
   */
  bool operator!=(const ConstDynImageRow& it) const noexcept
  {
    return row_index_ != it.row_index_ || img_ != it.img_;
  }

private:
  const DynImageView<modifiability>* img_;
  PixelIndex row_index_;

  ConstDynImageRow(const DynImageView<modifiability>* img, PixelIndex row_index) : img_(img), row_index_(row_index)
  {
  }
  friend class DynImageView<modifiability>;
  friend class ConstDynImageRowIterator<PixelType, modifiability>;
};


/** \brief Bidirectional iterator over rows of a (const) image.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType, ImageModifiability modifiability>
class ConstDynImageRowIterator
{
public:
  ~ConstDynImageRowIterator() = default;  ///< Destructor.
  ConstDynImageRowIterator(const ConstDynImageRowIterator<PixelType, modifiability>&) = default;  ///< Copy constructor.
  ConstDynImageRowIterator& operator=(const ConstDynImageRowIterator<PixelType, modifiability>&) = default;  ///< Copy assignment operator.
  ConstDynImageRowIterator(ConstDynImageRowIterator<PixelType, modifiability>&&) noexcept = default;  ///< Copy constructor.
  ConstDynImageRowIterator& operator=(ConstDynImageRowIterator<PixelType, modifiability>&&) noexcept = default;  ///< Copy assignment operator.

  /** \brief Iterator pre-decrement.
   *
   * @return Reference to *this.
   */
  ConstDynImageRowIterator<PixelType, modifiability>& operator--() noexcept
  {
    --row_.row_index_;
    return *this;
  }

  /** \brief Iterator pre-increment.
   *
   * @return Reference to *this.
   */
  ConstDynImageRowIterator<PixelType, modifiability>& operator++() noexcept
  {
    ++row_.row_index_;
    return *this;
  }

  /** \brief Iterator post-decrement.
   *
   * @return Reference to *this.
   */
  const ConstDynImageRowIterator<PixelType, modifiability> operator--(int) noexcept
  {
    ConstDynImageRowIterator it(*this);
    operator--();
    return it;
  }

  /** \brief Iterator post-increment.
   *
   * @return Reference to *this.
   */
  const ConstDynImageRowIterator<PixelType, modifiability> operator++(int) noexcept
  {
    ConstDynImageRowIterator it(*this);
    operator++();
    return it;
  }

  /** \brief Indicates iterator equality.
   *
   * @param it Another ConstDynImageRowIterator instance.
   * @return True, if both iterators reference the same row; false otherwise.
   */
  bool operator==(const ConstDynImageRowIterator<PixelType, modifiability>& it) const noexcept
  {
    return row_ == it.row_;
  }

  /** \brief Indicates iterator inequality.
   *
   * @param it Another ConstDynImageRowIterator instance.
   * @return True, if both iterators not reference the same row; false if they do.
   */
  bool operator!=(const ConstDynImageRowIterator<PixelType, modifiability>& it) const noexcept
  {
    return row_ != it.row_;
  }

  /** Iterator dereferencing operator.
   *
   * @return A `ConstDynImageRow` instance representing the respective row.
   */
  const ConstDynImageRow<PixelType, modifiability>& operator*() const noexcept
  {
    return row_;
  }

private:
  ConstDynImageRow<PixelType, modifiability> row_;

  explicit ConstDynImageRowIterator(ConstDynImageRow<PixelType, modifiability> row) : row_(row)
  {
  }

  friend class DynImageView<modifiability>;
};

/// @}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_DYN_IMAGE_ITERATORS_HPP
