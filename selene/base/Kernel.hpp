// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
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

/// \addtogroup group-base
/// @{

using KernelSize = std::ptrdiff_t;
constexpr static auto kernel_size_dynamic = KernelSize{-1};

template <typename ValueType_, KernelSize k_ = kernel_size_dynamic>
class Kernel;

/** \brief 1-dimensional kernel class.
 *
 * This class represents a 1-dimensional kernel, for use in image convolutions.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The kernel size. If it is set to `kernel_size_dynamic`, the data used to store the kernel elements
 *            will be allocated dynamically (i.e. using a `std::vector`); otherwise, it will be allocated on the stack
 *            (i.e. using a `std::array`).
 */
template <typename ValueType_, KernelSize k_>
class Kernel
{
public:
  using value_type = ValueType_;
  using iterator = typename std::array<ValueType_, k_>::iterator;
  using const_iterator = typename std::array<ValueType_, k_>::const_iterator;

  constexpr Kernel() = default;  ///< Default constructor.
  constexpr Kernel(const std::array<ValueType_, k_>& data);

  ~Kernel() = default;  ///< Defaulted destructor.

  constexpr Kernel(const Kernel&) = default;  ///< Defaulted copy constructor.
  constexpr Kernel& operator=(const Kernel&) = default;  ///< Defaulted copy assignment operator.
  constexpr Kernel(Kernel&&) noexcept = default;  ///< Defaulted move constructor.
  constexpr Kernel& operator=(Kernel&&) noexcept = default;  ///< Defaulted move assignment operator.

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  constexpr std::size_t size() const noexcept;
  constexpr value_type operator[](std::size_t idx) const noexcept;

  constexpr void normalize(value_type sum) noexcept;
  constexpr void normalize() noexcept;

private:
  std::array<ValueType_, k_> data_;
  static_assert(k_ >= 0, "Kernel size must be non-negative");
  static_assert(std::is_trivial_v<ValueType_>, "Value type of kernel is not trivial");
};


/** \brief 1-dimensional kernel class. Partial specialization for k_ == kernel_size_dynamic.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 */
template <typename ValueType_>
class Kernel<ValueType_, kernel_size_dynamic>
{
public:
  using value_type = ValueType_;
  using iterator = typename std::vector<ValueType_>::iterator;
  using const_iterator = typename std::vector<ValueType_>::const_iterator;

  Kernel() = default;  ///< Default constructor.
  Kernel(std::initializer_list<ValueType_> init);
  explicit Kernel(std::vector<value_type>&& vec);

  ~Kernel() = default;  ///< Defaulted destructor.

  Kernel(const Kernel&) = default;  ///< Defaulted copy constructor.
  Kernel& operator=(const Kernel&) = default;  ///< Defaulted copy assignment operator.
  Kernel(Kernel&&) noexcept = default;  ///< Defaulted move constructor.
  Kernel& operator=(Kernel&&) noexcept = default;  ///< Defaulted move assignment operator.

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  std::size_t size() const noexcept;
  value_type operator[](std::size_t idx) const noexcept;

  void normalize(value_type sum) noexcept;
  void normalize() noexcept;

private:
  std::vector<ValueType_> data_;
  static_assert(std::is_trivial_v<ValueType_>, "Value type of kernel is not trivial");
};

template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel, ValueType sum);

template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel);

template <KernelSize kernel_size, typename ValueType = default_float_t>
auto gaussian_kernel(default_float_t sigma, bool renormalize = true);

template <typename ValueType = default_float_t>
auto gaussian_kernel(default_float_t sigma, KernelSize size, bool renormalize = true);

template <typename ValueType = default_float_t>
auto gaussian_kernel(default_float_t sigma, default_float_t range_nr_std_deviations, bool renormalize = true);

template <KernelSize kernel_size, typename ValueType = default_float_t>
constexpr auto uniform_kernel();

template <typename ValueType = default_float_t>
auto uniform_kernel(KernelSize size);

template <typename OutValueType, std::ptrdiff_t scale_factor, typename ValueType, KernelSize k>
constexpr Kernel<OutValueType, k> integer_kernel(const Kernel<ValueType, k>& kernel);

template <typename OutValueType, std::ptrdiff_t scale_factor, typename ValueType>
Kernel<OutValueType, kernel_size_dynamic> integer_kernel(const Kernel<ValueType, kernel_size_dynamic>& kernel);

/// @}

// ----------
// Implementation:

/** \brief Constructor from a `std::array`.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @param data The data the kernel should contain.
 */
template <typename ValueType_, KernelSize k_>
constexpr Kernel<ValueType_, k_>::Kernel(const std::array<ValueType_, k_>& data)
    : data_(data)
{ }

/** \brief Returns an iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return An iterator to the beginning of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::begin() noexcept  -> iterator
{
  return data_.begin();
}

/** \brief Returns a constant iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return A constant iterator to the beginning of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::begin() const noexcept -> const_iterator
{
  return data_.begin();
}

/** \brief Returns a constant iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return A constant iterator to the beginning of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::cbegin() const noexcept -> const_iterator
{
  return data_.cbegin();
}

/** \brief Returns an iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return An iterator to the end of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::end() noexcept -> iterator
{
  return data_.end();
}

/** \brief Returns a constant iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return A constant iterator to the end of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::end() const noexcept -> const_iterator
{
  return data_.end();
}

/** \brief Returns a constant iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return A constant iterator to the end of the kernel data.
 */
template <typename ValueType_, KernelSize k_>
auto Kernel<ValueType_, k_>::cend() const noexcept -> const_iterator
{
  return data_.cend();
}

/** \brief Returns the size (length) of the kernel.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @return The kernel size.
 */
template <typename ValueType_, KernelSize k_>
constexpr std::size_t Kernel<ValueType_, k_>::size() const noexcept
{
  return static_cast<std::size_t>(k_);
}

/** \brief Access the n-th kernel element.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @param idx The index of the element to access.
 * @return The n-th kernel element, speficied by `idx`.
 */
template <typename ValueType_, KernelSize k_>
constexpr auto Kernel<ValueType_, k_>::operator[](std::size_t idx) const noexcept -> value_type
{
  SELENE_ASSERT(idx < k_);
  return data_[idx];
}

/** \brief Normalizes the kernel by dividing each element by the specified sum.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @param sum The value that each element will be divided by.
 */
template <typename ValueType_, KernelSize k_>
constexpr void Kernel<ValueType_, k_>::normalize(value_type sum) noexcept
{
  for (std::size_t i = 0; i < data_.size(); ++i)
  {
    data_[i] /= sum;
  }
}

/** \brief Normalizes the kernel such that the sum of (absolute) elements is 1.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 */
template <typename ValueType_, KernelSize k_>
constexpr void Kernel<ValueType_, k_>::normalize() noexcept
{
  auto abs_sum = value_type{0};
  for (std::size_t i = 0; i < data_.size(); ++i)
  {
    abs_sum += (data_[i] >= 0) ? data_[i] : -data_[i];
  }

  normalize(abs_sum);
}

// -----

/** \brief Constructor from an initializer list.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @param init The data the kernel should contain, in form of an initializer list.
 */
template <typename ValueType_>
Kernel<ValueType_, kernel_size_dynamic>::Kernel(std::initializer_list<ValueType_> init)
    : data_(init)
{
}

/** \brief Constructor from a `std::vector`.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @param vec The data the kernel should contain.
 */
template <typename ValueType_>
Kernel<ValueType_, kernel_size_dynamic>::Kernel(std::vector<value_type>&& vec)
    : data_(std::move(vec))
{
}

/** \brief Returns an iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return An iterator to the beginning of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::begin() noexcept -> iterator
{
  return data_.begin();
}

/** \brief Returns a constant iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return A constant iterator to the beginning of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::begin() const noexcept -> const_iterator
{
  return data_.begin();
}

/** \brief Returns a constant iterator to the beginning of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return A constant iterator to the beginning of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::cbegin() const noexcept -> const_iterator
{
  return data_.cbegin();
}

/** \brief Returns an iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return An iterator to the end of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::end() noexcept -> iterator
{
  return data_.end();
}

/** \brief Returns a constant iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return A constant iterator to the end of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::end() const noexcept -> const_iterator
{
  return data_.end();
}

/** \brief Returns a constant iterator to the end of the kernel data.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return A constant iterator to the end of the kernel data.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::cend() const noexcept -> const_iterator
{
  return data_.cend();
}

/** \brief Returns the size (length) of the kernel.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @return The kernel size.
 */
template <typename ValueType_>
std::size_t Kernel<ValueType_, kernel_size_dynamic>::size() const noexcept
{
  return data_.size();
}

/** \brief Access the n-th kernel element.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @param idx The index of the element to access.
 * @return The n-th kernel element, speficied by `idx`.
 */
template <typename ValueType_>
auto Kernel<ValueType_, kernel_size_dynamic>::operator[](std::size_t idx) const noexcept -> value_type
{
  SELENE_ASSERT(idx < data_.size());
  return data_[idx];
}

/** \brief Normalizes the kernel by dividing each element by the specified sum.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @param sum The value that each element will be divided by.
 */
template <typename ValueType_>
void Kernel<ValueType_, kernel_size_dynamic>::normalize(value_type sum) noexcept
{
  for (std::size_t i = 0; i < data_.size(); ++i)
  {
    data_[i] /= sum;
  }
}

/** \brief Normalizes the kernel such that the sum of (absolute) elements is 1.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 */
template <typename ValueType_>
void Kernel<ValueType_, kernel_size_dynamic>::normalize() noexcept
{
  auto abs_sum = value_type{0};
  for (std::size_t i = 0; i < data_.size(); ++i)
  {
    abs_sum += std::abs(data_[i]);
  }

  normalize(abs_sum);
}

// -----

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

/** \brief Returns a normalized kernel, where each element of the input kernel has been divided by the specified sum.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @param kernel The input kernel.
 * @param sum The value that each element of the input kernel will be divided by.
 * @return The normalized kernel.
 */
template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel, ValueType sum)
{
  auto normalized_kernel = kernel;
  normalized_kernel.normalize(sum);
  return normalized_kernel;
}

/** \brief Returns a normalized kernel, such that the sum of (absolute) elements is 1.
 *
 * @tparam ValueType_ The value type of the kernel elements.
 * @tparam k_ The number of kernel elements.
 * @param kernel The input kernel.
 * @return The normalized kernel.
 */
template <typename ValueType, KernelSize k>
constexpr Kernel<ValueType, k> normalize(const Kernel<ValueType, k>& kernel)
{
  auto normalized_kernel = kernel;
  normalized_kernel.normalize();
  return normalized_kernel;
}

/** \brief Returns a kernel discretely sampled from a Gaussian (normal) distribution.
 *
 * @tparam kernel_size The kernel size.
 * @tparam ValueType The value type of the kernel elements.
 * @param sigma The standard deviation of the Gaussian distribution.
 * @param renormalize If true, the kernel will be normalized after sampling, such the the sum of its elements is 1.
 * @return A kernel representing a sampled Gaussian distribution.
 */
template <KernelSize kernel_size, typename ValueType>
inline auto gaussian_kernel(default_float_t sigma, bool renormalize)
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

/** \brief Returns a kernel discretely sampled from a Gaussian (normal) distribution.
 *
 * @tparam ValueType The value type of the kernel elements.
 * @param sigma The standard deviation of the Gaussian distribution.
 * @param size The kernel size.
 * @param renormalize If true, the kernel will be normalized after sampling, such the the sum of its elements is 1.
 * @return A kernel representing a sampled Gaussian distribution.
 */
template <typename ValueType>
inline auto gaussian_kernel(default_float_t sigma, KernelSize size, bool renormalize)
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

/** \brief Returns a kernel discretely sampled from a Gaussian (normal) distribution.
 *
 * Using this overload, the kernel size will be determined by the given range in times of standard deviation.
 *
 * @tparam ValueType The value type of the kernel elements.
 * @param sigma The standard deviation of the Gaussian distribution.
 * @param range_nr_std_deviations How many standard deviations should be represented.
 * @param renormalize If true, the kernel will be normalized after sampling, such the the sum of its elements is 1.
 * @return A kernel representing a sampled Gaussian distribution.
 */
template <typename ValueType>
inline auto gaussian_kernel(default_float_t sigma, default_float_t range_nr_std_deviations, bool renormalize)
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

/** \brief Returns a kernel representing a discrete uniform distribution.
 *
 * @tparam kernel_size The kernel size.
 * @tparam ValueType The value type of the kernel elements.
 * @return A kernel representing a discrete uniform distribution.
 */
template <KernelSize kernel_size, typename ValueType>
constexpr auto uniform_kernel()
{
  static_assert(kernel_size > 0, "Kernel size must be >0.");

  constexpr auto value = ValueType(1.0) / ValueType(kernel_size);
  constexpr auto arr = sln::make_array_n_equal<ValueType, kernel_size>(value);
  return Kernel<ValueType, kernel_size>(arr);
}

/** \brief Returns a kernel representing a discrete uniform distribution.
 *
 * @tparam ValueType The value type of the kernel elements.
 * @param size The kernel size.
 * @return A kernel representing a discrete uniform distribution.
 */
template <typename ValueType>
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

/** \brief Converts a floating point kernel into a kernel containing scaled integral values.
 *
 * @tparam OutValueType The output element type of the kernel to be returned.
 * @tparam scale_factor The multiplication factor for scaling the input kernel elements with.
 * @tparam ValueType The value type of the input kernel elements.
 * @tparam k The kernel size.
 * @param kernel The input floating point kernel.
 * @return An integer kernel, scaled by the respective factor
 */
template <typename OutValueType, std::ptrdiff_t scale_factor, typename ValueType, KernelSize k>
constexpr auto integer_kernel(const Kernel<ValueType, k>& kernel) -> Kernel<OutValueType, k>
{
  static_assert(std::is_integral_v<OutValueType>, "Output type has to be integral");
  std::array<OutValueType, k> arr = {{OutValueType{}}};

  for (std::size_t i = 0; i < arr.size(); ++i)
  {
    arr[i] = sln::constexpr_round<OutValueType>(kernel[i] * scale_factor);
  }

  return Kernel<OutValueType, k>(arr);
}

/** \brief Converts a floating point kernel into a kernel containing scaled integral values.
 *
 * @tparam OutValueType The output element type of the kernel to be returned.
 * @tparam scale_factor The multiplication factor for scaling the input kernel elements with.
 * @tparam ValueType The value type of the input kernel elements.
 * @param kernel The input floating point kernel.
 * @return An integer kernel, scaled by the respective factor
 */
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
