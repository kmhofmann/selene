// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/ImageAccess.hpp>

#include "_TestImages.hpp"

using namespace sln::literals;


TEST_CASE("Image access convenience function", "[img]")
{
  const auto img = sln_test::make_3x3_test_image_8u1();

  // Floating point coordinates:

  // - without custom template arguments
  REQUIRE(sln::get(img, 0.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get(img, 1.0, 0.0) == Approx(20.0));
  REQUIRE(sln::get(img, 2.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get(img, 0.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get(img, 1.0, 1.0) == Approx(50.0));
  REQUIRE(sln::get(img, 2.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get(img, 0.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get(img, 1.0, 2.0) == Approx(80.0));
  REQUIRE(sln::get(img, 2.0, 2.0) == Approx(90.0));

  REQUIRE(sln::get(img, 0.5, 0.5) == Approx(30.0));
  REQUIRE(sln::get(img, 1.5, 0.5) == Approx(40.0));
  REQUIRE(sln::get(img, 0.5, 1.5) == Approx(60.0));
  REQUIRE(sln::get(img, 1.5, 1.5) == Approx(70.0));
  REQUIRE(sln::get(img, 0.5, 0.0) == Approx(15.0));
  REQUIRE(sln::get(img, 1.5, 0.0) == Approx(25.0));
  REQUIRE(sln::get(img, 1.0, 0.5) == Approx(35.0));
  REQUIRE(sln::get(img, 1.0, 1.5) == Approx(65.0));

  // - with custom interpolation mode
  //   - bilinear
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 0.0) == Approx(20.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 2.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 1.0) == Approx(50.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 2.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 2.0) == Approx(80.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 2.0, 2.0) == Approx(90.0));

  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.5, 0.5) == Approx(30.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.5, 0.5) == Approx(40.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.5, 1.5) == Approx(60.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.5, 1.5) == Approx(70.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.5, 0.0) == Approx(15.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.5, 0.0) == Approx(25.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 0.5) == Approx(35.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 1.5) == Approx(65.0));

  //   - nearest neighbor
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.0, 0.0) == Approx(20.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 2.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.0, 1.0) == Approx(50.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 2.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.0, 2.0) == Approx(80.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 2.0, 2.0) == Approx(90.0));

  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.5, 0.5) == Approx(50.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.5, 0.5) == Approx(60.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.5, 1.5) == Approx(80.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.5, 1.5) == Approx(90.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 0.5, 0.0) == Approx(20.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.5, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.0, 0.5) == Approx(50.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(img, 1.0, 1.5) == Approx(80.0));

  // - with custom border access mode
  //   - replication
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1.0, 0.0) == Approx(20.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1.0, 1.0) == Approx(50.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1.0, 2.0) == Approx(80.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2.0, 2.0) == Approx(90.0));

  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0.0, -1.0) == Approx(10.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1.0, -1.0) == Approx(20.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2.0, -1.0) == Approx(30.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3.0, 2.0) == Approx(90.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0.0, 3.0) == Approx(70.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1.0, 3.0) == Approx(80.0));
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2.0, 3.0) == Approx(90.0));

  //   - zero padding
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0.0, 0.0) == Approx(10.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1.0, 0.0) == Approx(20.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2.0, 0.0) == Approx(30.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0.0, 1.0) == Approx(40.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1.0, 1.0) == Approx(50.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2.0, 1.0) == Approx(60.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0.0, 2.0) == Approx(70.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1.0, 2.0) == Approx(80.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2.0, 2.0) == Approx(90.0));

  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1.0, 0.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1.0, 1.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1.0, 2.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0.0, -1.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1.0, -1.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2.0, -1.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3.0, 0.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3.0, 1.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3.0, 2.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0.0, 3.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1.0, 3.0) == Approx(0.0));
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2.0, 3.0) == Approx(0.0));

  // - with custom interpolation mode and border access mode (checks not exhaustive)
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 0.0)
          == Approx(10.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 0.0)
          == Approx(20.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 0.0)
          == Approx(30.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 1.0)
          == Approx(40.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 1.0)
          == Approx(50.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 1.0)
          == Approx(60.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 2.0)
          == Approx(70.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 2.0)
          == Approx(80.0));
  REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 2.0)
          == Approx(90.0));

  // ----------

  // Integer coordinates:

  // - without custom template arguments
  REQUIRE(sln::get(img, 0, 0) == 10);
  REQUIRE(sln::get(img, 1, 0) == 20);
  REQUIRE(sln::get(img, 2, 0) == 30);
  REQUIRE(sln::get(img, 0, 1) == 40);
  REQUIRE(sln::get(img, 1, 1) == 50);
  REQUIRE(sln::get(img, 2, 1) == 60);
  REQUIRE(sln::get(img, 0, 2) == 70);
  REQUIRE(sln::get(img, 1, 2) == 80);
  REQUIRE(sln::get(img, 2, 2) == 90);

  // - with custom border access mode
  //   - replication
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0, 0) == 10);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1, 0) == 20);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2, 0) == 30);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0, 1) == 40);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1, 1) == 50);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2, 1) == 60);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0, 2) == 70);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1, 2) == 80);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2, 2) == 90);

  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1, 0) == 10);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1, 1) == 40);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, -1, 2) == 70);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0, -1) == 10);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1, -1) == 20);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2, -1) == 30);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3, 0) == 30);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3, 1) == 60);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 3, 2) == 90);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 0, 3) == 70);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 1, 3) == 80);
  REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(img, 2, 3) == 90);

  //   - zero padding
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0, 0) == 10);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1, 0) == 20);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2, 0) == 30);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0, 1) == 40);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1, 1) == 50);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2, 1) == 60);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0, 2) == 70);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1, 2) == 80);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2, 2) == 90);

  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1, 0) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1, 1) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, -1, 2) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0, -1) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1, -1) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2, -1) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3, 0) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3, 1) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 3, 2) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 0, 3) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 1, 3) == 0);
  REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(img, 2, 3) == 0);
}