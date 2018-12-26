// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_KERNEL_HPP
#define SELENE_BASE_KERNEL_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/Round.hpp>
#include <selene/base/Types.hpp>
#include <selene/base/Utils.hpp>
#include <selene/base/_impl/ExplicitType.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <vector>

#include <cmath>

namespace sln {

using KernelSize = std::ptrdiff_t;
constexpr static auto kernel_size_dynamic = KernelSize{-1};

template <typename ValueType_, KernelSize k_ = kernel_size_dynamic>
class Kernel;

template <typename ValueType_, KernelSize k_>
class Kernel
{
public:
  using value_type = ValueType_;
  using iterator = typename std::array<ValueType_, k_>::iterator;
  using const_iterator = typename std::array<ValueType_, k_>::const_iterator;

  constexpr Kernel() = default;
  constexpr Kernel(const std::array<ValueType_, k_>& data) : data_(data)
  { }

  ~Kernel() = default;

  constexpr Kernel(const Kernel&) = default;
  constexpr Kernel& operator=(const Kernel&) = default;
  constexpr Kernel(Kernel&&) noexcept = default;
  constexpr Kernel& operator=(Kernel&&) noexcept = default;

  iterator begin() noexcept { return data_.begin(); }
  iterator begin() const noexcept { return data_.begin(); }
  const_iterator cbegin() const noexcept { return data_.cbegin(); }

  iterator end() noexcept { return data_.end(); }
  iterator end() const noexcept { return data_.end(); }
  const_iterator cend() const noexcept { return data_.cend(); }

  constexpr std::size_t size() const noexcept { return static_cast<std::size_t>(k_); }
  constexpr value_type operator[](std::size_t idx) const noexcept { return data_[idx]; }

  constexpr void normalize(value_type sum) noexcept
  {
    for (std::size_t i = 0; i < data_.size(); ++i)
    {
      data_[i] /= sum;
    }
  }

  constexpr void normalize() noexcept
  {
    auto abs_sum = value_type{0};
    for (std::size_t i = 0; i < data_.size(); ++i)
    {
      abs_sum += (data_[i] >= 0) ? data_[i] : -data_[i];
    }

    normalize(abs_sum);
  }

private:
  std::array<ValueType_, k_> data_;
  static_assert(k_ >= 0, "Kernel size must be non-negative");
  static_assert(std::is_trivial_v<ValueType_>, "Value type of kernel is not trivial");
};


template <typename ValueType_>
class Kernel<ValueType_, kernel_size_dynamic>
{
public:
  using value_type = ValueType_;
  using iterator = typename std::vector<ValueType_>::iterator;
  using const_iterator = typename std::vector<ValueType_>::const_iterator;

  Kernel() = default;
  Kernel(std::initializer_list<ValueType_> init) : data_(init)
  { }
  explicit Kernel(std::vector<value_type>&& vec) : data_(std::move(vec))
  { }

  ~Kernel() = default;

  Kernel(const Kernel&) = default;
  Kernel& operator=(const Kernel&) = default;
  Kernel(Kernel&&) noexcept = default;
  Kernel& operator=(Kernel&&) noexcept = default;

  iterator begin() noexcept { return data_.begin(); }
  iterator begin() const noexcept { return data_.begin(); }
  const_iterator cbegin() const noexcept { return data_.cbegin(); }

  iterator end() noexcept { return data_.end(); }
  iterator end() const noexcept { return data_.end(); }
  const_iterator cend() const noexcept { return data_.cend(); }

  std::size_t size() const noexcept { return data_.size(); }
  value_type operator[](std::size_t idx) const noexcept { return data_[idx]; }

  void normalize(value_type sum) noexcept
  {
    for (std::size_t i = 0; i < data_.size(); ++i)
    {
      data_[i] /= sum;
    }
  }

  void normalize() noexcept
  {
    auto abs_sum = value_type{0};
    for (std::size_t i = 0; i < data_.size(); ++i)
    {
      abs_sum += std::abs(data_[i]);
    }

    normalize(abs_sum);
  }

private:
  std::vector<ValueType_> data_;
  static_assert(std::is_trivial_v<ValueType_>, "Value type of kernel is not trivial");
};


template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel, ValueType sum)
{
  auto normalized_kernel = kernel;
  normalized_kernel.normalize(sum);
  return normalized_kernel;
}


template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel)
{
  auto normalized_kernel = kernel;
  normalized_kernel.normalize();
  return normalized_kernel;
}

// ----------

namespace impl {

template <typename ValueType = default_float_t>
inline auto gaussian_pdf(ValueType x, ValueType mu, ValueType sigma)
{
  constexpr auto f = ValueType(0.3989422804014326779); // 1.0 / sqrt(2.0 * M_PI)
  const auto diff = x - mu;
  return (f / sigma) * std::exp(-(diff * diff / (ValueType{2} * sigma * sigma)));
}

template <typename Container>
inline auto fill_with_gaussian_pdf(Container& c, std::ptrdiff_t center_idx, default_float_t sigma)
{
  using size_type = std::ptrdiff_t;
  using value_type = typename Container::value_type;
  auto sum = value_type{0};

  for (std::size_t i = 0; i < c.size(); ++i)
  {
    const auto x = value_type(static_cast<size_type>(i) - center_idx);
    const auto g = gaussian_pdf(x, value_type{0}, sigma);
    c[i] = g;
    sum += g;
  }

  return sum;
}

}  // namespace impl


template <KernelSize kernel_size, typename ValueType = default_float_t>
inline auto gaussian_kernel(default_float_t sigma, bool renormalize = true)
{
  static_assert(kernel_size % 2 == 1, "Gaussian kernel size must be odd");
  constexpr auto center_idx = kernel_size / 2;
  static_assert(center_idx == (kernel_size - 1) / 2);

  auto arr = std::array<ValueType, kernel_size>();
  const auto sum = impl::fill_with_gaussian_pdf(arr, center_idx, sigma);
  auto kernel = Kernel<ValueType, kernel_size>(arr);

  if (renormalize)
  {
    kernel.normalize(sum);
  }

  return kernel;
}

template <typename ValueType = default_float_t>
inline auto gaussian_kernel(default_float_t sigma, KernelSize size, bool renormalize = true)
{
  //SELENE_ASSERT(size % 2 == 1);
  const auto full_size = (size % 2 == 0) ? size + 1 : size;  // ensure kernel size is odd
  const auto center_idx = full_size / 2;
  SELENE_ASSERT(center_idx == (full_size - 1) / 2);

  auto vec = std::vector<ValueType>(static_cast<std::size_t>(full_size));
  const auto sum = impl::fill_with_gaussian_pdf(vec, center_idx, sigma);
  auto kernel = Kernel<ValueType, kernel_size_dynamic>(std::move(vec));

  if (renormalize)
  {
    kernel.normalize(sum);
  }

  return kernel;
}

template <typename ValueType = default_float_t>
inline auto gaussian_kernel(default_float_t sigma, default_float_t range_nr_std_deviations, bool renormalize = true)
{
  using size_type = std::ptrdiff_t;
  const auto half_size = static_cast<size_type>(std::ceil(sigma * range_nr_std_deviations));
  const auto full_size = 2 * std::max(size_type{1}, half_size) + 1;
  const auto center_idx = full_size / 2;
  SELENE_ASSERT(center_idx == (full_size - 1) / 2);

  auto vec = std::vector<ValueType>(static_cast<std::size_t>(full_size));
  const auto sum = impl::fill_with_gaussian_pdf(vec, center_idx, sigma);
  auto kernel = Kernel<ValueType, kernel_size_dynamic>(std::move(vec));

  if (renormalize)
  {
    kernel.normalize(sum);
  }

  return kernel;
}

template <KernelSize kernel_size, typename ValueType = default_float_t>
constexpr auto uniform_kernel()
{
  static_assert(kernel_size > 0, "Kernel size must be >0.");

  constexpr auto value = ValueType(1.0) / ValueType(kernel_size);
  constexpr auto arr = sln::make_array_n_equal<ValueType, kernel_size>(value);
  return Kernel<ValueType, kernel_size>(arr);
}

template <typename ValueType = default_float_t>
inline auto uniform_kernel(KernelSize size)
{
  if (size == 0)
  {
    return Kernel<ValueType, kernel_size_dynamic>();
  }

  const auto value = ValueType(1.0) / ValueType(size);
  auto vec = std::vector<ValueType>(static_cast<std::size_t>(size), value);
  return Kernel<ValueType, kernel_size_dynamic>(std::move(vec));
}

template <typename OutValueType, std::ptrdiff_t scale_factor, typename ValueType, KernelSize k>
constexpr auto integer_kernel(const Kernel<ValueType, k>& kernel) -> Kernel<OutValueType, k>
{
  static_assert(std::is_integral_v<OutValueType>, "Output type has to be integral");
  std::array<OutValueType, k> arr = {{OutValueType{}}};

  for (std::size_t i = 0; i < arr.size(); ++i)
  {
    // TODO: pessimized performance due to use of sln::constexpr_round?
    arr[i] = sln::constexpr_round<OutValueType>(kernel[i] * scale_factor);
  }

  return Kernel<OutValueType, k>(arr);
}

template <typename OutValueType, std::ptrdiff_t scale_factor, typename ValueType>
inline auto integer_kernel(const Kernel<ValueType, kernel_size_dynamic>& kernel) -> Kernel<OutValueType, kernel_size_dynamic>
{
  static_assert(std::is_integral_v<OutValueType>, "Output type has to be integral");
  std::vector<OutValueType> vec(kernel.size());

  for (std::size_t i = 0; i < vec.size(); ++i)
  {
    vec[i] = sln::round<OutValueType>(kernel[i] * scale_factor);
  }

  return Kernel<OutValueType, kernel_size_dynamic>(std::move(vec));
}

}  // namespace sln

#endif  // SELENE_BASE_KERNEL_HPP
