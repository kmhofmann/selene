// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_DYNIMAGE_HPP
#define SELENE_IMG2_DYNIMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img2/DynImageView.hpp>

namespace sln2 {

class DynImage
{
public:
  using DataPtrType = DataPtr<ImageModifiability::Mutable>::Type;

  explicit DynImage(UntypedLayout layout);
  ~DynImage();

  DynImage(const DynImage&);
  DynImage& operator=(const DynImage&);

  DynImage(DynImage&&) noexcept;
  DynImage& operator=(DynImage&&) noexcept;

  template <ImageModifiability modifiability> explicit DynImage(const DynImageView<modifiability>&);
  template <ImageModifiability modifiability> DynImage& operator=(const DynImageView<modifiability>&);

  UntypedLayout& layout() noexcept { return view_.layout(); }
  const UntypedLayout& layout() const noexcept { return view_.layout(); }

  PixelLength width() const noexcept { return view_.width(); }
  PixelLength height() const noexcept { return view_.height(); }
  std::int16_t nr_channels() const noexcept { return view_.nr_channels(); }
  std::int16_t nr_bytes_per_channel() const noexcept { return view_.nr_bytes_per_channel(); }
  Stride stride_bytes() const noexcept { return view_.stride_bytes(); }
  std::size_t row_bytes() const noexcept { return view_.row_bytes(); }
  std::size_t total_bytes() const noexcept { return view_.total_bytes(); }
  PixelFormat pixel_format() const noexcept { return view_.pixel_format(); }
  SampleFormat sample_format() const noexcept { return view_.sample_format(); }
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
  const DataPtrType byte_ptr() const noexcept { return view_.byte_ptr(); }

  DataPtrType byte_ptr(PixelIndex y) noexcept             { return view_.byte_ptr(y); }
  const DataPtrType byte_ptr(PixelIndex y) const noexcept { return view_.byte_ptr(y); }

  DataPtrType byte_ptr(PixelIndex x, PixelIndex y) noexcept             { return view_.byte_ptr(x, y); }
  const DataPtrType byte_ptr(PixelIndex x, PixelIndex y) const noexcept { return view_.byte_ptr(x, y); }

  template <typename PixelType>
  PixelType* data() noexcept             { return view_.data<PixelType>(); }

  template <typename PixelType>
  const PixelType* data() const noexcept { return view_.data<PixelType>(); }

  template <typename PixelType>
  PixelType* data(PixelIndex y) noexcept             { return view_.data<PixelType>(y); }

  template <typename PixelType>
  const PixelType* data(PixelIndex y) const noexcept { return view_.data<PixelType>(y); }

  template <typename PixelType>
  PixelType* data_row_end(PixelIndex y) noexcept             { return view_.data_row_end<PixelType>(y); }

  template <typename PixelType>
  const PixelType* data_row_end(PixelIndex y) const noexcept { return view_.data_row_end<PixelType>(y); }

  template <typename PixelType>
  PixelType* data(PixelIndex x, PixelIndex y) noexcept             { return view_.data<PixelType>(x, y); }

  template <typename PixelType>
  const PixelType* data(PixelIndex x, PixelIndex y) const noexcept { return view_.data<PixelType>(x, y); }

  template <typename PixelType>
  PixelType& pixel(PixelIndex x, PixelIndex y) noexcept             { return view_.pixel<PixelType>(x, y); }

  template <typename PixelType>
  const PixelType& pixel(PixelIndex x, PixelIndex y) const noexcept { return view_.pixel<PixelType>(x, y); }

  // Implicit conversion to the underlying view
  constexpr operator DynImageView<ImageModifiability::Mutable>&() noexcept
  {
    return view_;
  }

  // Implicit conversion to the underlying view
  constexpr operator const DynImageView<ImageModifiability::Mutable>&() const noexcept
  {
    return view_;
  }

private:
  constexpr static auto base_alignment_bytes = 16ul;
  DynImageView<ImageModifiability::Mutable> view_;

  void copy_rows_from(const DynImage& src);

  DynImageView<ImageModifiability::Mutable> allocate_memory(UntypedLayout layout, std::size_t alignment_bytes);
  void deallocate_memory();
};

inline DynImage::DynImage(UntypedLayout layout)
    : view_(this->allocate_memory(layout, base_alignment_bytes))
{
}

inline DynImage::~DynImage()
{
  this->deallocate_memory();
}

inline DynImage::DynImage(const DynImage& other)
    : view_(allocate_memory(other.layout(),
                           impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                     other.stride_bytes())))
{
  copy_rows_from(other);
}

inline DynImage& DynImage::operator=(const DynImage& other)
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

inline DynImage::DynImage(DynImage&& other) noexcept
    : view_(other.view_)
{
  other.view_ = DynImageView<ImageModifiability::Mutable>{{nullptr}, {PixelLength{0}, PixelLength{0}, std::int16_t{0}, std::int16_t{0}, Stride{0}}};
}

inline DynImage& DynImage::operator=(DynImage&& other) noexcept
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  // Clean up own memory
  this->deallocate_memory();

  view_ = other.view_;
  other.view_ = DynImageView<ImageModifiability::Mutable>{{nullptr}, {PixelLength{0}, PixelLength{0}, std::int16_t{0}, std::int16_t{0}, Stride{0}}};

  return *this;
}

template <ImageModifiability modifiability>
DynImage::DynImage(const DynImageView<modifiability>& other)
    : view_(allocate_memory(other.layout(),
                            impl::guess_row_alignment(reinterpret_cast<std::uintptr_t>(other.byte_ptr()),
                                                      other.stride_bytes())))
{
  copy_rows_from(other);
}

template <ImageModifiability modifiability>
DynImage& DynImage::operator=(const DynImageView<modifiability>& other)
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

inline void DynImage::copy_rows_from(const DynImage& src)
{
  SELENE_ASSERT(byte_ptr() && src.byte_ptr());
  SELENE_ASSERT(width() == src.width() && height() == src.height());

  for (PixelIndex y = 0_idx; y < view_.height(); ++y)
  {
    std::copy(src.byte_ptr(y), src.byte_ptr(y) + src.row_bytes(), byte_ptr(y));
  }
}

inline DynImageView<ImageModifiability::Mutable> DynImage::allocate_memory(UntypedLayout layout, std::size_t alignment_bytes)
{
  const auto stride_bytes = std::max(layout.stride_bytes(), Stride(layout.nr_bytes_per_channel() * layout.nr_channels() * layout.width()));
  const auto nr_bytes_to_allocate = stride_bytes * layout.height();

  auto memory = sln::AlignedNewAllocator::allocate(nr_bytes_to_allocate, alignment_bytes);
  SELENE_ASSERT(memory.size() == nr_bytes_to_allocate);

  return DynImageView<ImageModifiability::Mutable>{{memory.transfer_data()}, {layout.width(), layout.height(), layout.nr_channels(), layout.nr_bytes_per_channel(), stride_bytes}};
}

inline void DynImage::deallocate_memory()
{
  delete view_.byte_ptr();
}

}  // namespace sln

#endif  // SELENE_IMG2_DYNIMAGE_HPP