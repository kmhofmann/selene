// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>
#include <selene/base/Utils.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/Types.hpp>

#include <algorithm>
#include <memory>
#include <random>

#include <test/img/_TestImages.hpp>

using namespace sln::literals;

namespace {

std::vector<std::uint8_t> generate_random_data(std::size_t nr_bytes)
{
  std::independent_bits_engine<std::default_random_engine, 16, std::uint16_t> bytes_engine;
  std::vector<std::uint8_t> data(nr_bytes);
  std::for_each(data.begin(), data.end(), [&bytes_engine](auto& x){ x = static_cast<std::uint8_t>(bytes_engine()); });
  return data;
}

template <typename PixelType>
void test_image_data_construction(std::mt19937& rng)
{
  using ImageData = sln::ImageData<sln::ImageDataStorage::Modifiable>;
  using ConstImageData = sln::ImageData<sln::ImageDataStorage::Constant>;

  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = sln::PixelTraits<PixelType>::nr_bytes_per_channel;

  std::uniform_int_distribution<sln::PixelIndex::value_type> dist_wh(0, 100);  // width/height
  std::uniform_int_distribution<std::size_t> dist_algn(4, 6);  // log2(alignment_bytes)

  for (std::size_t i = 0; i < 50; ++i)
  {
    // Randomly determine image data properties
    const auto width = sln::PixelLength{dist_wh(rng)};
    const auto height = sln::PixelLength{dist_wh(rng)};
    const auto alignment_bytes = sln::power(std::size_t{2}, dist_algn(rng));

    ImageData img_data(width, height, nr_channels, nr_bytes_per_channel, sln::ImageRowAlignment{alignment_bytes});
    REQUIRE(img_data.width() == width);
    REQUIRE(img_data.height() == height);
    REQUIRE(img_data.nr_channels() == nr_channels);
    REQUIRE(img_data.nr_bytes_per_channel() == nr_bytes_per_channel);
    REQUIRE(img_data.stride_bytes() >= width * nr_channels * nr_bytes_per_channel);
    REQUIRE(img_data.row_bytes() == width * nr_channels * nr_bytes_per_channel);
    REQUIRE(img_data.total_bytes() >= width * height * nr_channels * nr_bytes_per_channel);
    REQUIRE(img_data.pixel_format() == sln::PixelFormat::Unknown);
    REQUIRE(img_data.sample_format() == sln::SampleFormat::Unknown);
    REQUIRE(!img_data.is_view());
    REQUIRE(img_data.is_valid() == (width * height > 0));

    // Generate some random data, and copy it as image data
    const auto data = generate_random_data(img_data.total_bytes());
    std::copy(data.cbegin(), data.cend(), img_data.byte_ptr());

    // Create a view onto the same data
    const auto stride_bytes = img_data.stride_bytes();
    ConstImageData cimg_data(data.data(), width, height, nr_channels, nr_bytes_per_channel, stride_bytes);
    REQUIRE(cimg_data.width() == width);
    REQUIRE(cimg_data.height() == height);
    REQUIRE(cimg_data.nr_channels() == nr_channels);
    REQUIRE(cimg_data.nr_bytes_per_channel() == nr_bytes_per_channel);
    REQUIRE(cimg_data.stride_bytes() >= width * nr_channels * nr_bytes_per_channel);
    REQUIRE(cimg_data.row_bytes() == width * nr_channels * nr_bytes_per_channel);
    REQUIRE(cimg_data.total_bytes() >= width * height * nr_channels * nr_bytes_per_channel);
    REQUIRE(cimg_data.pixel_format() == sln::PixelFormat::Unknown);
    REQUIRE(cimg_data.sample_format() == sln::SampleFormat::Unknown);
    REQUIRE(cimg_data.is_view());
    REQUIRE(cimg_data.is_valid() == (width * height > 0));

    REQUIRE(std::equal(img_data.byte_ptr(), img_data.byte_ptr() + img_data.total_bytes(), cimg_data.byte_ptr()));

    if (!img_data.is_valid())
    {
      continue;
    }

    // Check pixel access
    for (std::size_t j = 0; j < 100; ++j)
    {
      const auto x = sln::PixelLength{std::min(width - 1, dist_wh(rng))};
      const auto y = sln::PixelLength{std::min(height - 1, dist_wh(rng))};
      const auto& px0 = img_data.pixel<PixelType>(x, y);
      const auto& px1 = cimg_data.pixel<PixelType>(x, y);
      REQUIRE(px0 == px1);

      const auto* dptr = data.data() + y * img_data.stride_bytes() + x * nr_channels * nr_bytes_per_channel;
      const auto& px2 = *reinterpret_cast<const PixelType*>(dptr);
      REQUIRE(px0 == px2);
    }
  }
}

template <typename ElementType>
void test_image_data_construction_over_channels(std::mt19937& rng)
{
  test_image_data_construction<sln::Pixel<ElementType, 1>>(rng);
  test_image_data_construction<sln::Pixel<ElementType, 2>>(rng);
  test_image_data_construction<sln::Pixel<ElementType, 3>>(rng);
  test_image_data_construction<sln::Pixel<ElementType, 4>>(rng);
  test_image_data_construction<sln::Pixel<ElementType, 10>>(rng);
}

}  // namespace



TEST_CASE("Image data construction", "[img]")
{
  std::mt19937 rng(42);
  test_image_data_construction_over_channels<std::uint8_t>(rng);
  test_image_data_construction_over_channels<std::int8_t>(rng);
  test_image_data_construction_over_channels<std::uint16_t>(rng);
  test_image_data_construction_over_channels<std::int16_t>(rng);
  test_image_data_construction_over_channels<std::uint32_t>(rng);
  test_image_data_construction_over_channels<std::int32_t>(rng);
  test_image_data_construction_over_channels<std::uint64_t>(rng);
  test_image_data_construction_over_channels<std::int64_t>(rng);
}

