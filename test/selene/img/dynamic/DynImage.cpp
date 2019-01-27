// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img/pixel/PixelTraits.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <test/selene/Utils.hpp>
#include <test/selene/img/dynamic/_Utils.hpp>

#include <algorithm>
#include <memory>
#include <random>

using namespace sln::literals;

namespace {

template <typename PixelType>
void test_dyn_image_construction(std::mt19937& rng)
{
  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = sln::PixelTraits<PixelType>::nr_bytes_per_channel;

  std::uniform_int_distribution<sln::PixelIndex::value_type> dist_wh(0, 30);  // width/height
  std::uniform_int_distribution<std::size_t> dist_algn(4, 6);  // log2(alignment_bytes)

  for (std::size_t i = 0; i < 10; ++i)
  {
    // Randomly determine image data properties
    const auto width = sln::PixelLength{dist_wh(rng)};
    const auto height = sln::PixelLength{dist_wh(rng)};
    const auto alignment_bytes = sln::power(std::ptrdiff_t{2}, dist_algn(rng));

    sln::DynImage dyn_img{{width, height, nr_channels, nr_bytes_per_channel}, sln::ImageRowAlignment{alignment_bytes}};
    REQUIRE(dyn_img.width() == width);
    REQUIRE(dyn_img.height() == height);
    REQUIRE(dyn_img.nr_channels() == nr_channels);
    REQUIRE(dyn_img.nr_bytes_per_channel() == nr_bytes_per_channel);
    REQUIRE(dyn_img.stride_bytes() >= width * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img.row_bytes() == width * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img.total_bytes() >= width * height * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img.pixel_format() == sln::PixelFormat::Unknown);
    REQUIRE(dyn_img.sample_format() == sln::SampleFormat::Unknown);
    REQUIRE(dyn_img.is_valid() == (width * height > 0));

    // Generate some random data, and copy it as image data
    const auto data = sln_test::generate_random_data(static_cast<std::size_t>(dyn_img.total_bytes()));
    std::copy(data.cbegin(), data.cend(), dyn_img.byte_ptr());

    // Create a view onto the same data
    const auto stride_bytes = dyn_img.stride_bytes();
    sln::DynImageView<> dyn_img_view(data.data(), {width, height, nr_channels, nr_bytes_per_channel, stride_bytes});
    REQUIRE(dyn_img_view.width() == width);
    REQUIRE(dyn_img_view.height() == height);
    REQUIRE(dyn_img_view.nr_channels() == nr_channels);
    REQUIRE(dyn_img_view.nr_bytes_per_channel() == nr_bytes_per_channel);
    REQUIRE(dyn_img_view.stride_bytes() >= width * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img_view.row_bytes() == width * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img_view.total_bytes() >= width * height * nr_channels * nr_bytes_per_channel);
    REQUIRE(dyn_img_view.pixel_format() == sln::PixelFormat::Unknown);
    REQUIRE(dyn_img_view.sample_format() == sln::SampleFormat::Unknown);
    REQUIRE(dyn_img_view.is_valid() == (width * height > 0));

    REQUIRE(std::equal(dyn_img.byte_ptr(), dyn_img.byte_ptr() + dyn_img.total_bytes(), dyn_img_view.byte_ptr()));

    if (!dyn_img.is_valid())
    {
      continue;
    }

    // Check pixel access
    for (std::size_t j = 0; j < 100; ++j)
    {
      const auto x = std::min(sln::to_pixel_index(width - 1), sln::to_pixel_index(dist_wh(rng)));
      const auto y = std::min(sln::to_pixel_index(height - 1), sln::to_pixel_index(dist_wh(rng)));
      const auto& px0 = dyn_img.pixel<PixelType>(x, y);
      const auto& px1 = dyn_img_view.pixel<PixelType>(x, y);
      REQUIRE(px0 == px1);

      const auto* dptr = data.data() + y * dyn_img.stride_bytes() + x * nr_channels * nr_bytes_per_channel;
      const auto& px2 = *reinterpret_cast<const PixelType*>(dptr);
      REQUIRE(px0 == px2);
    }
  }
}

template <typename ElementType>
void test_dyn_image_construction_over_channels(std::mt19937& rng)
{
  test_dyn_image_construction<sln::Pixel<ElementType, 1>>(rng);
  test_dyn_image_construction<sln::Pixel<ElementType, 2>>(rng);
  test_dyn_image_construction<sln::Pixel<ElementType, 3>>(rng);
  test_dyn_image_construction<sln::Pixel<ElementType, 4>>(rng);
  test_dyn_image_construction<sln::Pixel<ElementType, 10>>(rng);
}

}  // namespace


TEST_CASE("DynImage construction", "[img]")
{
  std::mt19937 rng(42);
  test_dyn_image_construction_over_channels<std::uint8_t>(rng);
  test_dyn_image_construction_over_channels<std::int8_t>(rng);
  test_dyn_image_construction_over_channels<std::uint16_t>(rng);
  test_dyn_image_construction_over_channels<std::int16_t>(rng);
  test_dyn_image_construction_over_channels<std::uint32_t>(rng);
  test_dyn_image_construction_over_channels<std::int32_t>(rng);
  test_dyn_image_construction_over_channels<std::uint64_t>(rng);
  test_dyn_image_construction_over_channels<std::int64_t>(rng);
}

TEST_CASE("DynImage swap", "[img]")
{
  std::default_random_engine rng(43);
  auto dyn_img_0 = sln_test::construct_random_dynamic_image<sln::PixelRGB_8u>(20_px, 30_px, rng);
  auto dyn_img_1 = sln_test::construct_random_dynamic_image<sln::PixelY_16s>(30_px, 40_px, rng);

  constexpr std::size_t N{32}; // to fit within the first row of either

  const auto ptr_0 = dyn_img_0.byte_ptr();
  const auto layout_0 = dyn_img_0.layout();
  const auto semantics_0 = dyn_img_0.semantics();
  std::array<std::uint8_t, N> first_bytes_0;
  std::copy(dyn_img_0.byte_ptr(), dyn_img_0.byte_ptr() + N, first_bytes_0.begin());

  const auto ptr_1 = dyn_img_1.byte_ptr();
  const auto layout_1 = dyn_img_1.layout();
  const auto semantics_1 = dyn_img_1.semantics();
  std::array<std::uint8_t, N> first_bytes_1;
  std::copy(dyn_img_1.byte_ptr(), dyn_img_1.byte_ptr() + N, first_bytes_1.begin());

  using std::swap;
  swap(dyn_img_0, dyn_img_1);

  REQUIRE(dyn_img_0.byte_ptr() == ptr_1);
  REQUIRE(dyn_img_0.layout() == layout_1);
  REQUIRE(dyn_img_0.semantics() == semantics_1);
  REQUIRE(std::memcmp(dyn_img_0.byte_ptr(), first_bytes_1.data(), N) == 0);

  REQUIRE(dyn_img_1.byte_ptr() == ptr_0);
  REQUIRE(dyn_img_1.layout() == layout_0);
  REQUIRE(dyn_img_1.semantics() == semantics_0);
  REQUIRE(std::memcmp(dyn_img_1.byte_ptr(), first_bytes_0.data(), N) == 0);
}
