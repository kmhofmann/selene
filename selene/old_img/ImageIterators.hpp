// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_ITERATORS_HPP
#define SELENE_IMG_IMAGE_ITERATORS_HPP

/// @file

#include <selene/old_img/Types.hpp>

namespace sln {

template <typename PixelType>
class Image;

template <typename PixelType>
class ImageRowIterator;

template <typename PixelType>
class ConstImageRowIterator;

/** \brief Represents an image row (of a non-const image) whose elements can be iterated through.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
class ImageRow
{
public:
  ~ImageRow() = default;  ///< Destructor.
  ImageRow(const ImageRow&) = default;  ///< Copy constructor.
  ImageRow& operator=(const ImageRow&) = default;  ///< Copy assigment operator.
  ImageRow(ImageRow&&) noexcept = default;  ///< Copy constructor.
  ImageRow& operator=(ImageRow&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns an iterator to the first element of the image row.
   *
   * @return Iterator to the first row element.
   */
  PixelType* begin() noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* begin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* cbegin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns an iterator to the one-past-the-last element of the image row.
   *
   * @return Iterator to the one-past-the-last row element.
   */
  PixelType* end() noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* end() const noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* cend() const noexcept
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
  bool operator==(const ImageRow& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  /** \brief Indicates row inequality.
   *
   * @param it Another image row.
   * @return True, if both instances do not refer to the same image row; false if they do.
   */
  bool operator!=(const ImageRow& it) const noexcept
  {
    return row_index_ != it.row_index_ || img_ != it.img_;
  }

private:
  Image<PixelType>* img_;
  PixelIndex row_index_;

  ImageRow(Image<PixelType>* img, PixelIndex row_index) : img_(img), row_index_(row_index)
  {
  }
  friend class Image<PixelType>;
  friend class ImageRowIterator<PixelType>;
};


/** \brief Bidirectional iterator over rows of a (non-const) image.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
class ImageRowIterator
{
public:
  ~ImageRowIterator() = default;  ///< Destructor.
  ImageRowIterator(const ImageRowIterator&) = default;  ///< Copy constructor.
  ImageRowIterator& operator=(const ImageRowIterator&) = default;  ///< Copy assignment operator.
  ImageRowIterator(ImageRowIterator&&) noexcept = default;  ///< Move constructor.
  ImageRowIterator& operator=(ImageRowIterator&&) noexcept = default;  ///< Move assignment operator.

  /** \brief Iterator pre-decrement.
   *
   * @return Reference to *this.
   */
  ImageRowIterator& operator--() noexcept
  {
    --row_.row_index_;
    return *this;
  }

  /** \brief Iterator pre-increment.
   *
   * @return Reference to *this.
   */

  ImageRowIterator& operator++() noexcept
  {
    ++row_.row_index_;
    return *this;
  }

  /** \brief Iterator post-decrement.
   *
   * @return Reference to *this.
   */
  ImageRowIterator operator--(int) noexcept
  {
    ImageRowIterator it(*this);
    operator--();
    return it;
  }

  /** \brief Iterator post-increment.
   *
   * @return Reference to *this.
   */
  ImageRowIterator operator++(int) noexcept
  {
    ImageRowIterator it(*this);
    operator++();
    return it;
  }

  /** \brief Indicates iterator equality.
   *
   * @param it Another ImageRowIterator instance.
   * @return True, if both iterators reference the same row; false otherwise.
   */
  bool operator==(const ImageRowIterator& it) const noexcept
  {
    return row_ == it.row_;
  }

  /** \brief Indicates iterator inequality.
   *
   * @param it Another ImageRowIterator instance.
   * @return True, if both iterators not reference the same row; false if they do.
   */
  bool operator!=(const ImageRowIterator& it) const noexcept
  {
    return row_ != it.row_;
  }

  /** Iterator dereferencing operator.
   *
   * @return An `ImageRow` instance representing the respective row.
   */
  ImageRow<PixelType>& operator*() noexcept
  {
    return row_;
  }

private:
  ImageRow<PixelType> row_;

  explicit ImageRowIterator(ImageRow<PixelType> row) : row_(row)
  {
  }
  friend class Image<PixelType>;
};


/** \brief Represents an image row (of a const image) whose elements can be iterated through.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
class ConstImageRow
{
public:
  ~ConstImageRow() = default;  ///< Destructor.
  ConstImageRow(const ConstImageRow&) = default;  ///< Copy constructor.
  ConstImageRow& operator=(const ConstImageRow&) = default;  ///< Copy assigment operator.
  ConstImageRow(ConstImageRow&&) noexcept = default;  ///< Copy constructor.
  ConstImageRow& operator=(ConstImageRow&&) noexcept = default;  ///< Copy assigment operator.

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* begin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the first element of the image row.
   *
   * @return Const iterator to the first row element.
   */
  const PixelType* cbegin() const noexcept
  {
    return img_->data(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* end() const noexcept
  {
    return img_->data_row_end(row_index_);
  }

  /** \brief Returns a const iterator to the one-past-the-last element of the image row.
   *
   * @return Const iterator to the one-past-the-last row element.
   */
  const PixelType* cend() const noexcept
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
  bool operator==(const ConstImageRow& it) const noexcept
  {
    return row_index_ == it.row_index_ && img_ == it.img_;
  }

  /** \brief Indicates row inequality.
   *
   * @param it Another image row.
   * @return True, if both instances do not refer to the same image row; false if they do.
   */
  bool operator!=(const ConstImageRow& it) const noexcept
  {
    return row_index_ != it.row_index_ || img_ != it.img_;
  }

private:
  const Image<PixelType>* img_;
  PixelIndex row_index_;

  ConstImageRow(const Image<PixelType>* img, PixelIndex row_index) : img_(img), row_index_(row_index)
  {
  }
  friend class Image<PixelType>;
  friend class ConstImageRowIterator<PixelType>;
};


/** \brief Bidirectional iterator over rows of a (const) image.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
class ConstImageRowIterator
{
public:
  ~ConstImageRowIterator() = default;  ///< Destructor.
  ConstImageRowIterator(const ConstImageRowIterator&) = default;  ///< Copy constructor.
  ConstImageRowIterator& operator=(const ConstImageRowIterator&) = default;  ///< Copy assignment operator.
  ConstImageRowIterator(ConstImageRowIterator&&) noexcept = default;  ///< Copy constructor.
  ConstImageRowIterator& operator=(ConstImageRowIterator&&) noexcept = default;  ///< Copy assignment operator.

  /** \brief Iterator pre-decrement.
   *
   * @return Reference to *this.
   */
  ConstImageRowIterator& operator--() noexcept
  {
    --row_.row_index_;
    return *this;
  }

  /** \brief Iterator pre-increment.
   *
   * @return Reference to *this.
   */
  ConstImageRowIterator& operator++() noexcept
  {
    ++row_.row_index_;
    return *this;
  }

  /** \brief Iterator post-decrement.
   *
   * @return Reference to *this.
   */
  ConstImageRowIterator operator--(int) noexcept
  {
    ConstImageRowIterator it(*this);
    operator--();
    return it;
  }

  /** \brief Iterator post-increment.
   *
   * @return Reference to *this.
   */
  ConstImageRowIterator operator++(int) noexcept
  {
    ConstImageRowIterator it(*this);
    operator++();
    return it;
  }

  /** \brief Indicates iterator equality.
   *
   * @param it Another ConstImageRowIterator instance.
   * @return True, if both iterators reference the same row; false otherwise.
   */
  bool operator==(const ConstImageRowIterator& it) const noexcept
  {
    return row_ == it.row_;
  }

  /** \brief Indicates iterator inequality.
   *
   * @param it Another ConstImageRowIterator instance.
   * @return True, if both iterators not reference the same row; false if they do.
   */
  bool operator!=(const ConstImageRowIterator& it) const noexcept
  {
    return row_ != it.row_;
  }

  /** Iterator dereferencing operator.
   *
   * @return A `ConstImageRow` instance representing the respective row.
   */
  const ConstImageRow<PixelType>& operator*() const noexcept
  {
    return row_;
  }

private:
  ConstImageRow<PixelType> row_;

  explicit ConstImageRowIterator(ConstImageRow<PixelType> row) : row_(row)
  {
  }
  friend class Image<PixelType>;
};

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_ITERATORS_HPP
