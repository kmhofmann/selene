#include <cmath>
#include <cstdint>
#include <future>

#include <catch.hpp>

#include <selene/thread/ThreadPool.hpp>

double expensive_work(double seed)
{
  auto x = seed;

  for (std::size_t i = 0; i < 2500000; ++i)
  {
    x = std::pow(x, 1.1);
    x = std::exp(std::sin(x));
    x = std::exp(std::cos(x));
  }

  return x;
}

TEST_CASE("Simple ThreadPool test", "[thread]")
{
  sln::ThreadPool tp(8);
  REQUIRE(!tp.empty());
  REQUIRE(tp.size() == 8);

  std::vector<std::future<double>> futures;

  for (std::size_t i = 0; i < 8; ++i)
  {
    const auto seed = 5.0 * i;
    futures.emplace_back(tp.push(expensive_work, seed));
  }

  double sum = 0.0;
  for (auto& f : futures)
  {
    sum += f.get();
  }

  REQUIRE(sum == Approx(5.5223).margin(0.01));
}
