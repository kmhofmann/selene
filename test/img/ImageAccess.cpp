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

  SECTION("With floating point coordinates")
  {
    SECTION("Without template arguments")
    {
      REQUIRE(sln::get(img, 0.0, 0.0) == Approx(10.0));
      REQUIRE(sln::get(img, 1.0, 0.0) == Approx(20.0));
      REQUIRE(sln::get(img, 2.0, 0.0) == Approx(30.0));
      REQUIRE(sln::get(img, 0.0, 1.0) == Approx(40.0));
      REQUIRE(sln::get(img, 1.0, 1.0) == Approx(50.0));

      REQUIRE(sln::get(img, 0.5, 0.5) == Approx(30.0));
      REQUIRE(sln::get(img, 1.5, 0.5) == Approx(40.0));
      REQUIRE(sln::get(img, 0.5, 1.5) == Approx(60.0));
      REQUIRE(sln::get(img, 1.5, 1.5) == Approx(70.0));
      REQUIRE(sln::get(img, 0.5, 0.0) == Approx(15.0));
      REQUIRE(sln::get(img, 1.5, 0.0) == Approx(25.0));
      REQUIRE(sln::get(img, 1.0, 0.5) == Approx(35.0));
      REQUIRE(sln::get(img, 1.0, 1.5) == Approx(65.0));

      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);

        REQUIRE(sln::get(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));

        REQUIRE(sln::get(r_img, 0.5 - rx, 0.5 - ry) == Approx(30.0));
        REQUIRE(sln::get(r_img, 1.5 - rx, 0.5 - ry) == Approx(40.0));
        REQUIRE(sln::get(r_img, 0.5 - rx, 1.5 - ry) == Approx(60.0));
        REQUIRE(sln::get(r_img, 1.5 - rx, 1.5 - ry) == Approx(70.0));
        REQUIRE(sln::get(r_img, 0.5 - rx, 0.0 - ry) == Approx(15.0));
        REQUIRE(sln::get(r_img, 1.5 - rx, 0.0 - ry) == Approx(25.0));
        REQUIRE(sln::get(r_img, 1.0 - rx, 0.5 - ry) == Approx(35.0));
        REQUIRE(sln::get(r_img, 1.0 - rx, 1.5 - ry) == Approx(65.0));
      }
    }

    SECTION("With custom interpolation mode")
    {
      //   - bilinear
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.0, 0.0) == Approx(10.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 0.0) == Approx(20.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 2.0, 0.0) == Approx(30.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 0.0, 1.0) == Approx(40.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(img, 1.0, 1.0) == Approx(50.0));

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
      
      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);
        
        //   - bilinear
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));

        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 0.5 - rx, 0.5 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.5 - rx, 0.5 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 0.5 - rx, 1.5 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.5 - rx, 1.5 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 0.5 - rx, 0.0 - ry) == Approx(15.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.5 - rx, 0.0 - ry) == Approx(25.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.0 - rx, 0.5 - ry) == Approx(35.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear>(r_img, 1.0 - rx, 1.5 - ry) == Approx(65.0));

        //   - nearest neighbor
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 2.0 - rx, 1.0 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.0 - rx, 2.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.0 - rx, 2.0 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 2.0 - rx, 2.0 - ry) == Approx(90.0));

        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.5 - rx, 0.5 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.5 - rx, 0.5 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.5 - rx, 1.5 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.5 - rx, 1.5 - ry) == Approx(90.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 0.5 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.5 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.0 - rx, 0.5 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::NearestNeighbor>(r_img, 1.0 - rx, 1.5 - ry) == Approx(80.0));
      }
    }

    SECTION("With custom border access mode")
    {
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

      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);
        
        //   - replication
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 1.0 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 2.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 2.0 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 2.0 - ry) == Approx(90.0));

        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1.0 - rx, 2.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, -1.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, -1.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, -1.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3.0 - rx, 1.0 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3.0 - rx, 2.0 - ry) == Approx(90.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 3.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 3.0 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 3.0 - ry) == Approx(90.0));

        //   - zero padding
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2.0 - rx, 1.0 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0.0 - rx, 2.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1.0 - rx, 2.0 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2.0 - rx, 2.0 - ry) == Approx(90.0));

        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1.0 - rx, 0.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1.0 - rx, 1.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1.0 - rx, 2.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0.0 - rx, -1.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1.0 - rx, -1.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2.0 - rx, -1.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3.0 - rx, 0.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3.0 - rx, 1.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3.0 - rx, 2.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0.0 - rx, 3.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1.0 - rx, 3.0 - ry) == Approx(0.0));
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2.0 - rx, 3.0 - ry) == Approx(0.0));
      }
    }

    SECTION("With custom interpolation and border access mode")
    {
      // Checks not exhaustive
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 0.0) == Approx(10.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 0.0) == Approx(20.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 0.0) == Approx(30.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 1.0) == Approx(40.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 1.0) == Approx(50.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 1.0) == Approx(60.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 0.0, 2.0) == Approx(70.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 1.0, 2.0) == Approx(80.0));
      REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(img, 2.0, 2.0) == Approx(90.0));

      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);

        // Checks not exhaustive
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 0.0 - ry) == Approx(10.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 0.0 - ry) == Approx(20.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 0.0 - ry) == Approx(30.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 1.0 - ry) == Approx(40.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 1.0 - ry) == Approx(50.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 1.0 - ry) == Approx(60.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 0.0 - rx, 2.0 - ry) == Approx(70.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 1.0 - rx, 2.0 - ry) == Approx(80.0));
        REQUIRE(sln::get<sln::ImageInterpolationMode::Bilinear, sln::BorderAccessMode::Replicated>(r_img, 2.0 - rx, 2.0 - ry) == Approx(90.0));
      }
    }
  }

  SECTION("With integer coordinates")
  {
    SECTION("Without template arguments")
    {
      REQUIRE(sln::get(img, 0, 0) == 10);
      REQUIRE(sln::get(img, 1, 0) == 20);
      REQUIRE(sln::get(img, 2, 0) == 30);
      REQUIRE(sln::get(img, 0, 1) == 40);
      REQUIRE(sln::get(img, 1, 1) == 50);
      REQUIRE(sln::get(img, 2, 1) == 60);
      REQUIRE(sln::get(img, 0, 2) == 70);
      REQUIRE(sln::get(img, 1, 2) == 80);
      REQUIRE(sln::get(img, 2, 2) == 90);

      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);

        REQUIRE(sln::get(r_img, 0 - rx, 0 - ry) == 10);
        REQUIRE(sln::get(r_img, 1 - rx, 0 - ry) == 20);
        REQUIRE(sln::get(r_img, 2 - rx, 0 - ry) == 30);
        REQUIRE(sln::get(r_img, 0 - rx, 1 - ry) == 40);
        REQUIRE(sln::get(r_img, 1 - rx, 1 - ry) == 50);
        REQUIRE(sln::get(r_img, 2 - rx, 1 - ry) == 60);
        REQUIRE(sln::get(r_img, 0 - rx, 2 - ry) == 70);
        REQUIRE(sln::get(r_img, 1 - rx, 2 - ry) == 80);
        REQUIRE(sln::get(r_img, 2 - rx, 2 - ry) == 90);
      }
    }

    SECTION("With custom border access mode")
    {
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

      SECTION("Relative accessor")
      {
        const auto rx = 1;
        const auto ry = 1;
        const auto r_img = sln::relative_accessor(img, rx, ry);

        //   - replication
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0 - rx, 0 - ry) == 10);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1 - rx, 0 - ry) == 20);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2 - rx, 0 - ry) == 30);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0 - rx, 1 - ry) == 40);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1 - rx, 1 - ry) == 50);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2 - rx, 1 - ry) == 60);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0 - rx, 2 - ry) == 70);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1 - rx, 2 - ry) == 80);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2 - rx, 2 - ry) == 90);

        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1 - rx, 0 - ry) == 10);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1 - rx, 1 - ry) == 40);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, -1 - rx, 2 - ry) == 70);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0 - rx, -1 - ry) == 10);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1 - rx, -1 - ry) == 20);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2 - rx, -1 - ry) == 30);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3 - rx, 0 - ry) == 30);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3 - rx, 1 - ry) == 60);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 3 - rx, 2 - ry) == 90);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 0 - rx, 3 - ry) == 70);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 1 - rx, 3 - ry) == 80);
        REQUIRE(sln::get<sln::BorderAccessMode::Replicated>(r_img, 2 - rx, 3 - ry) == 90);

        //   - zero padding
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0 - rx, 0 - ry) == 10);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1 - rx, 0 - ry) == 20);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2 - rx, 0 - ry) == 30);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0 - rx, 1 - ry) == 40);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1 - rx, 1 - ry) == 50);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2 - rx, 1 - ry) == 60);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0 - rx, 2 - ry) == 70);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1 - rx, 2 - ry) == 80);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2 - rx, 2 - ry) == 90);

        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1 - rx, 0 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1 - rx, 1 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, -1 - rx, 2 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0 - rx, -1 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1 - rx, -1 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2 - rx, -1 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3 - rx, 0 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3 - rx, 1 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 3 - rx, 2 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 0 - rx, 3 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 1 - rx, 3 - ry) == 0);
        REQUIRE(sln::get<sln::BorderAccessMode::ZeroPadding>(r_img, 2 - rx, 3 - ry) == 0);
      }
    }
  }
}