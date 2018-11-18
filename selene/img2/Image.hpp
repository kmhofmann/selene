// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_IMAGE_HPP
#define SELENE_IMG2_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img2/ImageView.hpp>

namespace sln {

template <typename PixelType_>
class Image : public ImageBase<Image<PixelType_>>
{
public:
  using PixelType = PixelType_;
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;
  using ConstDataPtrType = DataPtr<ImageModifiability::Mutable>::ConstType;

  explicit Image(TypedLayout layout);
  Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes);
  ~Image();

  Image(const Image<PixelType>&);
  Image<PixelType>& operator=(const Image<PixelType>&);

  Image(Image<PixelType>&&) noexcept;
  Image<PixelType>& operator=(Image<PixelType>&&) noexcept;

  template <ImageModifiability modifiability> explicit Image(const ImageView<PixelType, modifiability>&);
  template <ImageModifiability modifiability> Image<PixelType>& operator=(const ImageView<PixelType, modifiability>&);

  TypedLayout& layout() noexcept { return view_.layout(); }
  const TypedLayout& layout() const noexcept { return view_.layout(); }

  PixelLength width() const noexcept { return view_.width(); }
  PixelLength height() const noexcept { return view_.height(); }
  Stride stride_bytes() const noexcept { return view_.stride_bytes(); }
  std::ptrdiff_t row_bytes() const noexcept { return view_.row_bytes(); }
  std::ptrdiff_t total_bytes() const noexcept { return view_.total_bytes(); }
  bool is_packed() const noexcept { return view_.is_packed(); }

  bool is_empty() const noexcept { return view_.is_empty(); }
  bool is_valid() const noexcept { return view_.is_valid(); };

//  iterator begin() noexcept;
//  const_iterator begin() const noexcept;
//  const_iterator cbegin() const noexcept;
//
//  iterator end() noexcept;
//  const_iterator end() const noexcept;
//  const_iterator cend() const noexcept;

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

  ImageView<PixelType, ImageModifiability::Mutable>& view() noexcept { return view_; }
  const ImageView<PixelType, ImageModifiability::Mutable>& view() const noexcept { return view_; }

  // Implicit conversion to the underlying view
  operator ImageView<PixelType, ImageModifiability::Mutable>&() noexcept
  {
    return view_;
  }

  // Implicit conversion to the underlying view
  operator const ImageView<PixelType, ImageModifiability::Mutable>&() const noexcept
  {
    return view_;
  }

private:
  constexpr static auto base_alignment_bytes = 16ul;
  ImageView<PixelType, ImageModifiability::Mutable> view_;

  template <typename PixelTypeSrc> void copy_rows_from(const Image<PixelTypeSrc>& src);

  ImageView<PixelType, ImageModifiability::Mutable> allocate_memory(TypedLayout layout, std::size_t alignment_bytes);
  void deallocate_memory();
};

template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout)
    : view_(this->allocate_memory(layout, base_alignment_bytes))
{
}

template <typename PixelType_>
Image<PixelType_>::Image(TypedLayout layout, ImageRowAlignment row_alignment_bytes)
    : view_(this->allocate_memory(layout, row_alignment_bytes))
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
template <ImageModifiability modifiability>
Image<PixelType_>::Image(const ImageView<PixelType, modifiability>& other)
    : view_(allocate_memory(other.layout(),
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.data()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

template <typename PixelType_>
template <ImageModifiability modifiability>
Image<PixelType_>& Image<PixelType_>::operator=(const ImageView<PixelType, modifiability>& other)
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
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes()));
  }

  copy_rows_from(other);

  return *this;
}

template <typename PixelType_>
template <typename PixelTypeSrc>
void Image<PixelType_>::copy_rows_from(const Image<PixelTypeSrc>& src)
{
  SELENE_ASSERT(data() && src.data());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < view_.height(); ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType_>
ImageView<PixelType_, ImageModifiability::Mutable> Image<PixelType_>::allocate_memory(TypedLayout layout, std::size_t alignment_bytes)
{
  const auto stride_bytes = std::max(layout.stride_bytes(), Stride(PixelTraits<PixelType>::nr_bytes * layout.width()));
  const auto nr_bytes_to_allocate = stride_bytes * layout.height();

  auto memory = sln::AlignedNewAllocator::allocate(nr_bytes_to_allocate, alignment_bytes);
  SELENE_ASSERT(static_cast<std::ptrdiff_t>(memory.size()) == nr_bytes_to_allocate);

  return ImageView<PixelType, ImageModifiability::Mutable>{{memory.transfer_data()}, {layout.width(), layout.height(), stride_bytes}};
}

template <typename PixelType_>
void Image<PixelType_>::deallocate_memory()
{
  std::uint8_t* ptr = view_.byte_ptr();
  sln::AlignedNewAllocator::deallocate(ptr);
}

}  // namespace sln

#endif  // SELENE_IMG2_IMAGE_HPP
