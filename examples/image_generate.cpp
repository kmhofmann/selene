// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/png/Write.hpp>

#include <selene/img_ops/Generate.hpp>

#include <cmath>
#include <complex>

#include "Utils.hpp"

constexpr auto output_filename_mandelbrot = "mandelbrot.png";
constexpr auto output_filename_collatz = "collatz.png";

using namespace sln::literals;

double mandelbrot(double px, double py)
{
  constexpr int N = 1024;
  double x = 0.0;
  double y = 0.0;

  for (int i = 1; i < N; ++i)
  {
    const auto x2 = x * x;
    const auto y2 = y * y;
    const auto xy = x * y;

    x = x2 - y2 + px;
    y = 2 * xy + py;

    if (x * x + y * y > 4.0)
    {
      return double(i) / double(N);
    }
  }

  return 0.0;
}

double collatz(double px, double py)
{
  constexpr int N = 128;
  std::complex<double> z{px, py};

  for (int i = 0; i < N; ++i)
  {
    z = 0.25 * (1.0 + 4.0 * z - (1.0 + 2.0 * z) * std::cos(M_PI * z));

    if (std::abs(z.imag()) > 16.0)
    {
      return std::log(i + 1.0) / std::log(N);
    }
  }

  return 0.0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  constexpr auto width = 1200_px;
  constexpr auto height = 800_px;

  constexpr auto col0 = sln::PixelRGB_8u(0, 0, 255);
  constexpr auto col1 = sln::PixelRGB_8u(0, 0, 0);

  auto func_mandelbrot = [width, height, col0, col1](sln::PixelIndex x, sln::PixelIndex y) {
    const double fx = double{x} / double{width} * 3.5 - 2.5;
    const double fy = double{y} / double{height} * 2.0 - 1.0;
    const auto f = std::pow(mandelbrot(fx, fy), 0.2);

    return static_cast<sln::PixelRGB_8u>(f * col0 + (1.0 - f) * col1);
  };

  auto func_collatz = [width, height, col0, col1](sln::PixelIndex x, sln::PixelIndex y) {
    const double fx = double{x} / double{width} * 5.0 - 2.5;
    const double fy = double{y} / double{height} * 3.0 - 1.5;
    const auto f = std::pow(collatz(fx, fy), 0.5);

    return static_cast<sln::PixelRGB_8u>(f * col0 + (1.0 - f) * col1);
  };

  std::cout << "Generating Mandelbrot fractal...\n";
  const auto mandelbrot_img = sln::generate(func_mandelbrot, width, height);

  std::cout << "Generating & evaluating Collatz fractal expression...\n";
  const auto collatz_expr = sln::generate_expr(func_collatz, width, height);
  const auto collatz_img = collatz_expr.eval();

  std::cout << "Writing images...\n";
  sln::write_png(sln::to_dyn_image_view(mandelbrot_img), sln::FileWriter(output_filename_mandelbrot));
  sln::write_png(sln::to_dyn_image_view(collatz_img), sln::FileWriter(output_filename_collatz));

  return 0;
}
