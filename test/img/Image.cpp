// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Allocators.hpp>
#include <selene/base/Types.hpp>
#include <selene/img/Image.hpp>

#include <random>

#include "_TestImages.hpp"

using namespace sln::literals;

namespace {

template <typename T>
void basic_image_tests(sln::PixelLength width, sln::PixelLength height, T fill_value)
{
  sln::Image<T> img(width, height);
  REQUIRE(img.width() == width);
  REQUIRE(img.height() == height);
  REQUIRE(img.stride_bytes() == img.width() * sizeof(T));
  REQUIRE(img.is_packed());
  REQUIRE(!img.is_view());
  REQUIRE(!img.is_empty());

  img.fill(fill_value);
  for (auto y = 0_px; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (auto x = 0_px; x < img.width(); ++x)
    {
      REQUIRE(img(x, y) == fill_value);
      REQUIRE(*(ptr + x) == fill_value);
    }
  }

  sln::Image<T> img2;
  clone(img, img2);
  REQUIRE(img2.width() == img.width());
  REQUIRE(img2.height() == img.height());
  REQUIRE(img2.stride_bytes() == img.stride_bytes());
  REQUIRE(img2.is_packed());
  REQUIRE(!img2.is_view());
  REQUIRE(!img2.is_empty());

  sln::Image<T> img3;
  img3.set_view(img.byte_ptr(), img.width(), img.height(), img.stride_bytes());
  REQUIRE(img3.width() == img.width());
  REQUIRE(img3.height() == img.height());
  REQUIRE(img3.stride_bytes() == img.stride_bytes());
  REQUIRE(img3.is_packed());
  REQUIRE(img3.is_view());
  REQUIRE(!img3.is_empty());

  img3.maybe_allocate(img3.width(), img3.height(),
                      sln::Stride{img3.stride_bytes() + 1000});  // stride should be ignored
  REQUIRE(img3.width() == img.width());
  REQUIRE(img3.height() == img.height());
  REQUIRE(img3.stride_bytes() == img.stride_bytes());
  REQUIRE(img3.is_packed());
  REQUIRE(img3.is_view());
  REQUIRE(!img3.is_empty());

  REQUIRE_THROWS(img3.maybe_allocate(sln::PixelLength{img3.width() + 1}, sln::PixelLength{img3.height() + 1}));

  REQUIRE_NOTHROW(
      img2.maybe_allocate(img2.width(), sln::PixelLength{img2.height() + 1}, sln::Stride{img2.stride_bytes() + 16}));
  REQUIRE(img2.width() == img.width());
  REQUIRE(img2.height() == img.height() + 1);
  REQUIRE(img2.stride_bytes() == img.stride_bytes() + 16);
  REQUIRE(!img2.is_packed());
  REQUIRE(!img2.is_view());
  REQUIRE(!img2.is_empty());

  constexpr auto test_width = sln::PixelLength{16};
  constexpr auto test_height = sln::PixelLength{20};
  constexpr auto test_stride_offset = 8;
  constexpr auto test_stride_bytes = sln::Stride{test_width * sln::PixelTraits<T>::nr_bytes + test_stride_offset};
  const auto nr_bytes_to_allocate = test_stride_bytes * test_height;
  auto memory_block = sln::AlignedNewAllocator::allocate(nr_bytes_to_allocate, 16);
  img.set_data(std::move(memory_block), test_width, test_height, test_stride_bytes);

  REQUIRE(img.width() == test_width);
  REQUIRE(img.height() == test_height);
  REQUIRE(img.stride_bytes() == test_stride_bytes);
  REQUIRE(!img.is_packed());
  REQUIRE(!img.is_view());
  REQUIRE(!img.is_empty());

  for (std::size_t alignment = 1; alignment <= 128; alignment <<= 1)
  {
    sln::Image<T> img4(width, height, sln::ImageRowAlignment{alignment});
    REQUIRE(img4.width() == width);
    REQUIRE(img4.height() == height);
    REQUIRE(img4.stride_bytes() % alignment == 0);
    REQUIRE(!img4.is_view());
    REQUIRE(!img4.is_empty());
    for (auto y = 0_px; y < img4.height(); ++y)
    {
      REQUIRE(reinterpret_cast<std::uintptr_t>(img4.data(y)) % alignment == 0);
    }

    const auto alignment2 = alignment / 2;
    img4.allocate(sln::PixelLength{width + 1}, sln::PixelLength{height + 1}, sln::ImageRowAlignment{alignment2});
    REQUIRE(img4.width() == width + 1);
    REQUIRE(img4.height() == height + 1);
    REQUIRE((alignment2 == 0 || img4.stride_bytes() % alignment2 == 0));
    REQUIRE(!img4.is_view());
    REQUIRE(!img4.is_empty());
    for (auto y = 0_px; y < img4.height(); ++y)
    {
      REQUIRE((alignment2 == 0 || reinterpret_cast<std::uintptr_t>(img4.data(y)) % alignment2 == 0));
    }
  }
}

template <typename T, typename RNG>
sln::Image<T> construct_random_image(sln::PixelLength width, sln::PixelLength height, RNG& rng)
{
  static_assert(sln::PixelTraits<T>::nr_channels == 1, "Unexpected multi-channel image");

  using Element = typename sln::PixelTraits<T>::Element;

  constexpr auto is_int = sln::PixelTraits<T>::is_integral;
  constexpr auto is_fp = sln::PixelTraits<T>::is_floating_point;
  auto die = sln_test::uniform_distribution<Element>(Element{0},
                                                     is_int ? std::numeric_limits<Element>::max() : Element{1});

  std::uniform_int_distribution<std::uint16_t> die_stride(0, 16);
  const auto extra_stride_bytes = std::size_t{die_stride(rng) * sizeof(Element)};
  const auto stride_bytes = sln::Stride(width * sln::PixelTraits<T>::nr_bytes + extra_stride_bytes);
  sln::Image<T> img(width, height, stride_bytes);

  for (auto y = 0_px; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (auto x = 0_px; x < img.width(); ++x)
    {
      *ptr++ = T(die(rng));
    }
  }

  return img;
}

template <typename T>
void compare_iteration(sln::Image<T>& img)
{
  // const iteration

  {
    const auto& img2 = img;

    std::vector<T> elements_0;
    for (auto y = 0_px; y < img2.height(); ++y)
    {
      for (auto x = 0_px; x < img2.width(); ++x)
      {
        elements_0.emplace_back(img2(x, y));
      }
    }

    std::uint64_t y_count{0};
    std::vector<T> elements_1;
    for (const auto& row : img2)
    {
      REQUIRE(row.index() == y_count);
      for (const auto& el : row)
      {
        elements_1.emplace_back(el);
      }
      ++y_count;
    }

    std::vector<T> elements_2;
    for (typename sln::Image<T>::const_iterator it = img2.cbegin(); it != img2.cend(); ++it)
    {
      const sln::ConstImageRow<T>& row = *it;
      for (auto it_el = row.cbegin(); it_el != row.cend(); ++it_el)
      {
        const T& el = *it_el;
        elements_2.emplace_back(el);
      }
    }

    REQUIRE(elements_0 == elements_1);
    REQUIRE(elements_0 == elements_2);
  }

  // non-const iteration

  {
    std::vector<T> elements_0;
    for (auto y = 0_px; y < img.height(); ++y)
    {
      for (auto x = 0_px; x < img.width(); ++x)
      {
        elements_0.emplace_back(img(x, y));
      }
    }

    std::uint64_t y_count{0};
    std::vector<T> elements_1;
    for (auto& row : img)
    {
      REQUIRE(row.index() == y_count);
      for (auto& el : row)
      {
        elements_1.emplace_back(el);
      }
      ++y_count;
    }

    std::vector<T> elements_2;
    for (typename sln::Image<T>::iterator it = img.begin(); it != img.end(); ++it)
    {
      sln::ImageRow<T>& row = *it;
      for (auto it_el = row.begin(); it_el != row.end(); ++it_el)
      {
        T& el = *it_el;
        elements_2.emplace_back(el);
        el = T{17};  // overwrite image element
      }
    }

    // check that elements have all been overwritten
    for (auto y = 0_px; y < img.height(); ++y)
    {
      for (auto x = 0_px; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == T{17});
      }
    }

    REQUIRE(elements_0 == elements_1);
    REQUIRE(elements_0 == elements_2);
  }
}

template <typename ElementType, typename RNG>
void random_iteration(sln::PixelIndex w, sln::PixelIndex h, RNG& rng)
{
  auto img = construct_random_image<sln::Pixel<ElementType, 1>>(w, h, rng);
  compare_iteration(img);
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

TEST_CASE("Image iteration", "[img]")
{
  std::mt19937 rng(42ul);

  for (auto h = 1_px; h < 16_px; ++h)
  {
    for (auto w = 1_px; w < 16_px; ++w)
    {
      random_iteration<std::uint8_t>(w, h, rng);
      random_iteration<std::int8_t>(w, h, rng);
      random_iteration<std::uint16_t>(w, h, rng);
      random_iteration<std::int16_t>(w, h, rng);
      random_iteration<std::uint32_t>(w, h, rng);
      random_iteration<std::int32_t>(w, h, rng);
      random_iteration<std::uint64_t>(w, h, rng);
      random_iteration<std::int64_t>(w, h, rng);
      random_iteration<sln::float32_t>(w, h, rng);
      random_iteration<sln::float64_t>(w, h, rng);
    }
  }
}

TEST_CASE("Image comparison", "[img]")
{
  const auto img_x = sln_test::make_3x3_test_image_8u1();
  const auto img_x_2 = clone(img_x);
  REQUIRE(img_x_2.width() == img_x.width());
  REQUIRE(img_x_2.height() == img_x.height());
  REQUIRE(img_x_2 == img_x);

  const auto img_xxx = sln_test::make_3x3_test_image_8u3();
  const auto img_xxx_2 = clone(img_xxx);
  REQUIRE(img_xxx_2.width() == img_xxx.width());
  REQUIRE(img_xxx_2.height() == img_xxx.height());
  REQUIRE(img_xxx_2 == img_xxx);
}
