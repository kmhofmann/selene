// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMAGE_HPP
#define SELENE_IMG_TYPED_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/typed/ImageView.hpp>

#include <cstring>

namespace sln {

template <typename PixelType_>
class Image;

template <typename PixelType_>
struct ImageBaseTraits<Image<PixelType_>>
{
  using PixelType = PixelType_;

  constexpr static bool is_view = false;
  constexpr static bool is_modifiable = true;

  constexpr static ImageModifiability modifiability()
  {
    return ImageModifiability::Mutable;
  }
};

template <typename PixelType_>
class Image : public ImageBase<Image<PixelType_>>
{
public:
  using PixelType = PixelType_;
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  using iterator = ImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The iterator type.
  using const_iterator = ConstImageRowIterator<PixelType, ImageModifiability::Mutable>;  ///< The const_iterator type.

  constexpr static bool is_view = ImageBaseTraits<Image<PixelType>>::is_view;
  constexpr static bool is_modifiable = ImageBaseTraits<Image<PixelType>>::is_modifiable;
  constexpr static ImageModifiability modifiability() { return ImageBaseTraits<Image<PixelType>>::modifiability(); }

  Image() = default;
  explicit Image(TypedLayout layout);
  Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes);
  Image(MemoryBlock<AlignedNewAllocator>&& memory, TypedLayout layout);
  ~Image();

  Image(const Image<PixelType>&);
  Image<PixelType>& operator=(const Image<PixelType>&);

  Image(Image<PixelType>&&) noexcept;
  Image<PixelType>& operator=(Image<PixelType>&&) noexcept;

  template <ImageModifiability modifiability> explicit Image(const ImageView<PixelType, modifiability>&);
  template <ImageModifiability modifiability> Image<PixelType>& operator=(const ImageView<PixelType, modifiability>&);

  const TypedLayout& layout() const noexcept { return view_.layout(); }

  PixelLength width() const noexcept { return view_.width(); }
  PixelLength height() const noexcept { return view_.height(); }
  Stride stride_bytes() const noexcept { return view_.stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return view_.row_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return view_.total_bytes(); }
  bool is_packed() const noexcept { return view_.is_packed(); }

  bool is_empty() const noexcept { return view_.is_empty(); }
  bool is_valid() const noexcept { return view_.is_valid(); };

  iterator begin() noexcept { return view_.begin(); }
  const_iterator begin() const noexcept { return view_.begin(); }
  const_iterator cbegin() const noexcept { return view_.cbegin(); }

  iterator end() noexcept { return view_.end(); }
  const_iterator end() const noexcept { return view_.end(); }
  const_iterator cend() const noexcept { return view_.cend(); }

  DataPtrType byte_ptr() noexcept             { return view_.byte_ptr(); }
  ConstDataPtrType byte_ptr() const noexcept { return view_.byte_ptr(); }

  DataPtrType byte_ptr(PixelIndex y) noexcept             { return view_.byte_ptr(y); }
  ConstDataPtrType byte_ptr(PixelIndex y) const noexcept { return view_.byte_ptr(y); }

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept             { return view_.byte_ptr(x, y); }
  ConstDataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return view_.byte_ptr(x, y); }

  PixelType* data() noexcept             { return view_.data(); }
  const PixelType* data() const noexcept { return view_.data(); }

  PixelType* data(PixelIndex y) noexcept             { return view_.data(y); }
  const PixelType* data(PixelIndex y) const noexcept { return view_.data(y); }

  PixelType* data_row_end(PixelIndex y) noexcept             { return view_.data_row_end(y); }
  const PixelType* data_row_end(PixelIndex y) const noexcept { return view_.data_row_end(y); }

  PixelType* data(PixelIndex x, PixelIndex y) noexcept             { return view_.data(x, y); }
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept { return view_.data(x, y); }

  PixelType& operator()(PixelIndex x, PixelIndex y) noexcept             { return view_.operator()(x, y); }
  const PixelType& operator()(PixelIndex x, PixelIndex y) const noexcept { return view_.operator()(x, y); }

  ImageView<PixelType, ImageModifiability::Mutable>& view() noexcept { return view_.view(); }
  ImageView<PixelType, ImageModifiability::Constant> view() const noexcept { return view_.constant_view(); }
  ImageView<PixelType, ImageModifiability::Constant> constant_view() const noexcept { return view_.constant_view(); }

  void clear()
  {
    deallocate_memory();
    view_.clear();
  }

  bool reallocate(TypedLayout layout, ImageRowAlignment row_alignment_bytes, bool shrink_to_fit = true);
  MemoryBlock<AlignedNewAllocator> relinquish_data_ownership();

private:
  constexpr static auto default_base_alignment_bytes = ImageRowAlignment{16ul};

  ImageView<PixelType, ImageModifiability::Mutable> view_;

  template <typename Derived> void copy_rows_from(const ImageBase<Derived>& src);

  ImageView<PixelType, ImageModifiability::Mutable> allocate_memory(TypedLayout layout, std::ptrdiff_t base_alignment_bytes, std::ptrdiff_t row_alignment_bytes);
  void deallocate_memory();
};

template <typename PixelType>
bool operator==(const Image<PixelType>& img0, const Image<PixelType>& img1);

template <typename PixelType>
bool operator!=(const Image<PixelType>& img0, const Image<PixelType>& img1);

// ----------
// Implementation:

template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, 0))
{
}

template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes)
    : view_(this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes))
{
}

template <typename PixelType_>
Image<PixelType_>::Image(MemoryBlock<AlignedNewAllocator>&& memory, TypedLayout layout)
    : view_(memory.transfer_data(), layout)
{
}

template <typename PixelType_>
Image<PixelType_>::~Image()
{
  this->deallocate_memory();
}

template <typename PixelType_>
Image<PixelType_>::Image(const Image<PixelType>& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

template <typename PixelType_>
Image<PixelType_>& Image<PixelType_>::operator=(const Image<PixelType>& other)
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    view_ = allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()));
  }

  copy_rows_from(other);

  return *this;
}

template <typename PixelType_>
Image<PixelType_>::Image(Image<PixelType>&& other) noexcept
    : view_(other.view_)
{
  other.view_ = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr}, {PixelLength{0}, PixelLength{0}, Stride{0}}};
}

template <typename PixelType_>
Image<PixelType_>& Image<PixelType_>::operator=(Image<PixelType>&& other) noexcept
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_ = other.view_;
  other.view_ = ImageView<PixelType, ImageModifiability::Mutable>{{nullptr}, {PixelLength{0}, PixelLength{0}, Stride{0}}};

  return *this;
}

template <typename PixelType_>
template <ImageModifiability modifiability_>
Image<PixelType_>::Image(const ImageView<PixelType, modifiability_>& other)
    : view_(allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

template <typename PixelType_>
template <ImageModifiability modifiability_>
Image<PixelType_>& Image<PixelType_>::operator=(const ImageView<PixelType, modifiability_>& other)
{
  // Check for self-assignment
  if (&this->view_ == &other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  if (!equal_size)
  {
    // Clean up own memory
    this->deallocate_memory();

    // Allocate new memory
    view_ = allocate_memory(other.layout(),
                            default_base_alignment_bytes,
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()));
  }

  copy_rows_from(other);

  return *this;
}

template <typename PixelType_>
bool Image<PixelType_>::reallocate(TypedLayout layout, ImageRowAlignment row_alignment_bytes, bool shrink_to_fit)
{
  if (layout == this->view_.layout())
  {
    return false;
  }

  layout.stride_bytes = impl::compute_stride_bytes(std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width)), row_alignment_bytes);
  const auto nr_bytes_to_allocate = layout.stride_bytes * layout.height;
  const auto nr_currently_allocated_bytes = this->stride_bytes() * this->height();

  // No need to act if size parameters match
  const auto bytes_match = shrink_to_fit ? (nr_bytes_to_allocate == nr_currently_allocated_bytes)
                                         : (nr_bytes_to_allocate <= nr_currently_allocated_bytes);
  if (bytes_match)
  {
    view_ = ImageView<PixelType, ImageModifiability::Mutable>(this->byte_ptr(), layout);
    return false;
  }

  this->deallocate_memory();
  view_ = this->allocate_memory(layout, default_base_alignment_bytes, row_alignment_bytes);
  return true;
}

template <typename PixelType_>
MemoryBlock<AlignedNewAllocator> Image<PixelType_>::relinquish_data_ownership()
{
  const auto ptr = this->byte_ptr();
  const auto len = this->total_bytes();

  view_.clear();
  return construct_memory_block_from_existing_memory<AlignedNewAllocator>(ptr, static_cast<std::size_t>(len));
}

template <typename PixelType_>
template <typename Derived>
void Image<PixelType_>::copy_rows_from(const ImageBase<Derived>& src)
{
  SELENE_ASSERT(data() && src.data());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < view_.height(); ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType_>
ImageView<PixelType_, ImageModifiability::Mutable> Image<PixelType_>::allocate_memory(TypedLayout layout, std::ptrdiff_t base_alignment_bytes, std::ptrdiff_t row_alignment_bytes)
{
  const auto stride_bytes = impl::compute_stride_bytes(std::max(layout.stride_bytes, Stride(PixelTraits<PixelType>::nr_bytes * layout.width)), row_alignment_bytes);
  const auto nr_bytes_to_allocate = stride_bytes * layout.height;

  base_alignment_bytes = std::max(row_alignment_bytes, base_alignment_bytes);
  auto memory = sln::AlignedNewAllocator::allocate(static_cast<std::size_t>(nr_bytes_to_allocate), static_cast<std::size_t>(base_alignment_bytes));
  SELENE_ASSERT(static_cast<std::ptrdiff_t>(memory.size()) == nr_bytes_to_allocate);

  return ImageView<PixelType, ImageModifiability::Mutable>{{memory.transfer_data()}, {layout.width, layout.height, stride_bytes}};
}

template <typename PixelType_>
void Image<PixelType_>::deallocate_memory()
{
  std::uint8_t* ptr = view_.byte_ptr();
  sln::AlignedNewAllocator::deallocate(ptr);
}

// -----

template <typename PixelType>
bool operator==(const Image<PixelType>& img0, const Image<PixelType>& img1)
{
  // Special case: if both images have a zero-length side, the shall be considered equal (both are invalid)
  if ((img0.width() == 0 || img0.height() == 0) && (img1.width() == 0 || img1.height() == 0))
  {
    return true;
  }

  if (img0.width() != img1.width() || img0.height() != img1.height())
  {
    return false;
  }

  for (auto y = 0_idx; y < img0.height(); ++y)
  {
    const auto end0 = img0.data_row_end(y);
    const auto begin0 = img0.data(y);
    const auto begin1 = img1.data(y);

    // std::equal may not be optimized to std::memcmp, even though we're dealing with a POD-type here...
    // const bool equal_row = std::equal(begin0, end0, begin1);
    // ...so let's just call std::memcmp directly:
    const auto nr_bytes = std::distance(begin0, end0) * sizeof(PixelType);
    const bool equal_row = (std::memcmp(begin0, begin1, nr_bytes) == 0);

    if (!equal_row)
    {
      return false;
    }
  }

  return true;
}

template <typename PixelType>
bool operator!=(const Image<PixelType>& img0, const Image<PixelType>& img1)
{
  return !(img0 == img1);
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMAGE_HPP
