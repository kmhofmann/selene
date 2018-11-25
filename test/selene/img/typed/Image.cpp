// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/ImageFunctions.hpp>

//#include <test/selene/img/typed/Utils.hpp>

using namespace sln::literals;

namespace {

template <typename T>
void basic_image_tests(sln::PixelLength width, sln::PixelLength height, T fill_value)
{
  {
    sln::Image<T> img;
  }

  sln::Image<T> img0({width, height});
  REQUIRE(img0.width() == width);
  REQUIRE(img0.height() == height);
  REQUIRE(img0.stride_bytes() == img0.width() * static_cast<std::ptrdiff_t>(sizeof(T)));
  REQUIRE(img0.is_packed());
  REQUIRE(!img0.is_empty());

  sln::fill(img0, fill_value);
  for (auto y = 0_idx; y < img0.height(); ++y)
  {
    auto ptr = img0.data(y);
    for (auto x = 0_idx; x < img0.width(); ++x)
    {
      REQUIRE(img0(x, y) == fill_value);
      REQUIRE(*(ptr + x) == fill_value);
    }
  }

  sln::Image<T> img1 = img0;
  REQUIRE(img1.width() == img0.width());
  REQUIRE(img1.height() == img0.height());
  REQUIRE(img1.stride_bytes() == img0.stride_bytes());
  REQUIRE(img1.is_packed());
  REQUIRE(!img1.is_empty());

//  sln::ImageView<T> img3{{img.byte_ptr()}, {img.width(), img.height(), img.stride_bytes()}};
//  REQUIRE(img3.width() == img.width());
//  REQUIRE(img3.height() == img.height());
//  REQUIRE(img3.stride_bytes() == img.stride_bytes());
//  REQUIRE(img3.is_packed());
//  REQUIRE(!img3.is_empty());

  //img3.maybe_allocate(img3.width(), img3.height(),
  //                    sln::Stride{img3.stride_bytes() + 1000});  // stride should be ignored
  //REQUIRE(img3.width() == img.width());
  //REQUIRE(img3.height() == img.height());
  //REQUIRE(img3.stride_bytes() == img.stride_bytes());
  //REQUIRE(img3.is_packed());
  //REQUIRE(img3.is_view());
  //REQUIRE(!img3.is_empty());

  //REQUIRE_THROWS(img3.maybe_allocate(sln::PixelLength{img3.width() + 1}, sln::PixelLength{img3.height() + 1}));

  //REQUIRE_NOTHROW(
  //    img.maybe_allocate(img.width(), sln::PixelLength{img.height() + 1}, sln::Stride{img.stride_bytes() + 16}));
  //REQUIRE(img.width() == img.width());
  //REQUIRE(img.height() == img.height() + 1);
  //REQUIRE(img.stride_bytes() == img.stride_bytes() + 16);
  //REQUIRE(!img.is_packed());
  //REQUIRE(!img.is_view());
  //REQUIRE(!img.is_empty());

  //constexpr auto test_width = sln::PixelLength{16};
  //constexpr auto test_height = sln::PixelLength{20};
  //constexpr auto test_stride_offset = 8;
  //constexpr auto test_stride_bytes = sln::Stride(test_width * sln::PixelTraits<T>::nr_bytes + test_stride_offset);
  //const auto nr_bytes_to_allocate = test_stride_bytes * test_height;
  //auto memory_block = sln::AlignedNewAllocator::allocate(nr_bytes_to_allocate, 16);
  //img.set_data(std::move(memory_block), test_width, test_height, test_stride_bytes);

  //REQUIRE(img.width() == test_width);
  //REQUIRE(img.height() == test_height);
  //REQUIRE(img.stride_bytes() == test_stride_bytes);
  //REQUIRE(!img.is_packed());
  //REQUIRE(!img.is_view());
  //REQUIRE(!img.is_empty());

  for (std::size_t alignment = 1; alignment <= 128; alignment <<= 1)
  {
    sln::Image<T> img({width, height}, sln::ImageRowAlignment{alignment});
    REQUIRE(img.width() == width);
    REQUIRE(img.height() == height);
    REQUIRE(img.stride_bytes() % alignment == 0);
    REQUIRE(!img.is_empty());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      REQUIRE(reinterpret_cast<std::uintptr_t>(img.data(y)) % alignment == 0);
    }

    const auto alignment2 = alignment / 2;
    img.reallocate({sln::PixelLength{width + 1}, sln::PixelLength{height + 1}}, sln::ImageRowAlignment{alignment2});
    REQUIRE(img.width() == width + 1);
    REQUIRE(img.height() == height + 1);
    REQUIRE((alignment2 == 0 || img.stride_bytes() % alignment2 == 0));
    REQUIRE(!img.is_empty());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      REQUIRE((alignment2 == 0 || reinterpret_cast<std::uintptr_t>(img.data(y)) % alignment2 == 0));
    }
  }
}

}  // namespace


TEST_CASE("Image construction", "[img]")
{
  constexpr auto fill_value = 42;
  for (auto h = 10_px; h < 100_px; h += 20_px)
  {
    for (auto w = 10_px; w < 100_px; w += 10_px)
    {
      basic_image_tests<std::uint8_t>(w, h, fill_value);
      basic_image_tests<std::int8_t>(w, h, fill_value);
      basic_image_tests<std::uint16_t>(w, h, fill_value);
      basic_image_tests<std::int16_t>(w, h, fill_value);
      basic_image_tests<std::uint32_t>(w, h, fill_value);
      basic_image_tests<std::int32_t>(w, h, fill_value);
      basic_image_tests<std::uint64_t>(w, h, fill_value);
      basic_image_tests<std::int64_t>(w, h, fill_value);
      basic_image_tests<sln::float32_t>(w, h, sln::float32_t{fill_value});
      basic_image_tests<sln::float64_t>(w, h, sln::float64_t{fill_value});
    }
  }
}

//TEST_CASE("Image comparison", "[img]")
//{
//  const auto img_x = sln_test::make_3x3_test_image_8u1();
//  const auto img_x_2 = clone(img_x);
//  REQUIRE(img_x_2.width() == img_x.width());
//  REQUIRE(img_x_2.height() == img_x.height());
//  REQUIRE(img_x_2 == img_x);

//  const auto img_xxx = sln_test::make_3x3_test_image_8u3();
//  const auto img_xxx_2 = clone(img_xxx);
//  REQUIRE(img_xxx_2.width() == img_xxx.width());
//  REQUIRE(img_xxx_2.height() == img_xxx.height());
//  REQUIRE(img_xxx_2 == img_xxx);
//}

//TEST_CASE("Image view creation", "[img]")
//{
//  auto img0 = sln_test::make_3x3_test_image_8u1();
//  auto img0_view = sln::view(img0);

//  REQUIRE(img0(1_idx, 1_idx) == 50);
//  img0_view(1_idx, 1_idx) = 42;
//  REQUIRE(img0(1_idx, 1_idx) == 42);

//  const auto img1 = sln_test::make_3x3_test_image_8u1();
//  auto img1_view = sln::view(img1);

//  REQUIRE(img1(1_idx, 1_idx) == 50);
//  //img1_view(1_idx, 1_idx) = 42;
//  //REQUIRE(img1(1_idx, 1_idx) == 42);
//}

