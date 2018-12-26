// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/base/Kernel.hpp>

#include <numeric>
#include <random>

// ---
#include <iostream>

TEST_CASE("Kernel", "[base]")
{
  SECTION("Empty kernel")
  {
    sln::Kernel<double> k;
    REQUIRE(k.size() == 0);
  }

  SECTION("Empty kernel (static)")
  {
    constexpr static sln::Kernel<double, 0> k;
    STATIC_REQUIRE(k.size() == 0);
  }

  SECTION("Kernel initializer list")
  {
    const sln::Kernel<double> k{{42.0, 66.6, 80.01}};
    REQUIRE(k.size() == 3);
    REQUIRE(k[0] == 42.0);
    REQUIRE(k[1] == 66.6);
    REQUIRE(k[2] == 80.01);

    const auto k2 = sln::normalize(k);
    REQUIRE(k2[0] == Approx(42.0 / (42.0 + 66.6 + 80.01)));
    REQUIRE(k2[1] == Approx(66.6 / (42.0 + 66.6 + 80.01)));
    REQUIRE(k2[2] == Approx(80.01 / (42.0 + 66.6 + 80.01)));
  };

  SECTION("Kernel initializer list (static)")
  {
    constexpr static sln::Kernel<double, 3> k{{42.0, 66.6, 80.01}};
    STATIC_REQUIRE(k.size() == 3);
    STATIC_REQUIRE(k[0] == 42.0);
    STATIC_REQUIRE(k[1] == 66.6);
    STATIC_REQUIRE(k[2] == 80.01);

    constexpr static auto k2 = sln::normalize(k);
    STATIC_REQUIRE(k2[0] == 42.0 / (42.0 + 66.6 + 80.01));
    STATIC_REQUIRE(k2[1] == 66.6 / (42.0 + 66.6 + 80.01));
    STATIC_REQUIRE(k2[2] == 80.01 / (42.0 + 66.6 + 80.01));
  };
}

TEST_CASE("Gaussian kernel", "[base]")
{
  SECTION("Static version")
  {
    const auto sgk = sln::gaussian_kernel<7>(sln::default_float_t(1.0));
    REQUIRE(sgk.size() == 7);
    REQUIRE(sgk[0] == Approx(0.00443305));
    REQUIRE(sgk[1] == Approx(0.0540056));
    REQUIRE(sgk[2] == Approx(0.242036));
    REQUIRE(sgk[3] == Approx(0.39905));
    REQUIRE(sgk[4] == Approx(sgk[2]));
    REQUIRE(sgk[5] == Approx(sgk[1]));
    REQUIRE(sgk[6] == Approx(sgk[0]));

    for (auto sigma = sln::default_float_t{0.001}; sigma < 3.0; sigma *= 1.1)
    {
      const auto sgk = sln::gaussian_kernel<5>(sigma);
      REQUIRE(sgk.size() == 5);
    }
  }

  SECTION("Dynamic version")
  {
    {
      const auto gk = sln::gaussian_kernel(sln::default_float_t(1.0), sln::default_float_t(3.0));
      REQUIRE(gk.size() == 7);
      REQUIRE(gk[0] == Approx(0.00443305));
      REQUIRE(gk[1] == Approx(0.0540056));
      REQUIRE(gk[2] == Approx(0.242036));
      REQUIRE(gk[3] == Approx(0.39905));
      REQUIRE(gk[4] == Approx(gk[2]));
      REQUIRE(gk[5] == Approx(gk[1]));
      REQUIRE(gk[6] == Approx(gk[0]));

      for (auto sigma = sln::default_float_t{0.001}; sigma < 3.0; sigma *= 1.1)
      {
        const auto gk = sln::gaussian_kernel(sigma, sln::default_float_t(3.0));
        REQUIRE(gk.size() > 0);
      }
    }

    {
      const auto gk = sln::gaussian_kernel(sln::default_float_t(1.0), sln::KernelSize{7});
      REQUIRE(gk.size() == 7);
      REQUIRE(gk[0] == Approx(0.00443305));
      REQUIRE(gk[1] == Approx(0.0540056));
      REQUIRE(gk[2] == Approx(0.242036));
      REQUIRE(gk[3] == Approx(0.39905));
      REQUIRE(gk[4] == Approx(gk[2]));
      REQUIRE(gk[5] == Approx(gk[1]));
      REQUIRE(gk[6] == Approx(gk[0]));

      for (auto sigma = sln::default_float_t{0.001}; sigma < 3.0; sigma *= 1.1)
      {
        const auto gk = sln::gaussian_kernel(sigma, sln::KernelSize{7});
        REQUIRE(gk.size() == 7);
      }
    }
  }
}

template <sln::KernelSize kernel_size> void test_uniform_static_kernel()
{
  const auto uk = sln::uniform_kernel<kernel_size>();
  STATIC_REQUIRE(uk.size() == kernel_size);

  const auto value = (kernel_size == 0) ? 0 : sln::default_float_t(1.0) / uk.size();
  for (std::size_t x = 0; x < uk.size(); ++x)
  {
    REQUIRE(uk[x] == value);
  }
}

TEST_CASE("Uniform kernel", "[base]")
{
  SECTION("Static version")
  {
    test_uniform_static_kernel<1>();
    test_uniform_static_kernel<2>();
    test_uniform_static_kernel<3>();
    test_uniform_static_kernel<4>();
    test_uniform_static_kernel<5>();
    // ...
  }

  SECTION("Dynamic version")
  {
    for (sln::KernelSize i = 0; i < 10; ++i)
    {
      const auto uk = sln::uniform_kernel(i);
      REQUIRE(static_cast<sln::KernelSize>(uk.size()) == i);

      const auto value = (i == 0) ? 0 : sln::default_float_t(1.0) / uk.size();
      for (std::size_t x = 0; x < uk.size(); ++x)
      {
        REQUIRE(uk[x] == value);
      }
    }
  }
}

template <std::ptrdiff_t scale_factor, typename IntegerType, typename RNG>
void test_integer_kernel(RNG& rng)
{
  for (std::size_t iter = 0; iter < 1000; ++iter)
  {
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    std::vector<double> vec(5);
    std::generate(vec.begin(), vec.end(), [&](){ return dist(rng); });
    const auto k = sln::Kernel<double>(std::move(vec));
    const auto nk = sln::normalize(k);
    const auto ik = sln::integer_kernel<IntegerType, scale_factor>(nk);
    REQUIRE(k.size() == nk.size());
    REQUIRE(k.size() == ik.size());

    const auto nsum = std::accumulate(nk.cbegin(), nk.cend(), 0.0,
                                     [](auto l, auto r){ return std::abs(l) + std::abs(r); });
    REQUIRE(nsum == Approx(1.0));

    const auto sum = std::accumulate(ik.cbegin(), ik.cend(), IntegerType{0},
                                     [](auto l, auto r){ return std::abs(l) + std::abs(r); });
    REQUIRE(sum >= scale_factor - 2);  // some empirical slack...
    REQUIRE(sum <= scale_factor + 2);
  }
}

TEST_CASE("Integer kernel", "[base]")
{
  SECTION("Static version")
  {
    constexpr auto scale_factor = std::ptrdiff_t{256};
    constexpr auto k = sln::Kernel<double, 7>({1.0, -26.1, 3.0, 40.0, 5.0, 66.0, -7.5});
    constexpr auto nk = sln::normalize(k);
    constexpr auto ik = sln::integer_kernel<std::int32_t, scale_factor>(nk);
    STATIC_REQUIRE(k.size() == nk.size());
    STATIC_REQUIRE(k.size() == ik.size());

    const auto sum = std::accumulate(ik.cbegin(), ik.cend(), std::int32_t{0},
                                     [](auto l, auto r){ return std::abs(l) + std::abs(r); });
    REQUIRE(sum >= scale_factor - 1);
    REQUIRE(sum <= scale_factor + 1);
  }

  SECTION("Dynamic version")
  {
    std::mt19937 rng(142);
    test_integer_kernel<256, std::int32_t>(rng);
    test_integer_kernel<65536, std::int32_t>(rng);
    test_integer_kernel<4294967296, std::int64_t>(rng);
  }
}