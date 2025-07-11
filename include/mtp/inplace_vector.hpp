#ifndef MTP_INPLACE_VECTOR_HPP
#define MTP_INPLACE_VECTOR_HPP

#if __cplusplus <= 201703L
#  error "Requires c++20 or later."
#endif

#ifndef MTP_EXPORT
#  define MTP_EXPORT
#endif

#ifndef MTP_EXPECTS
#  if defined(_MSC_VER) && !defined(__clang__)
#    define MTP_EXPECTS(cond) __assume(cond)
#  elif defined(__GNUC__) || defined(__clang__)
#    define MTP_EXPECTS(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#  else
#    define MTP_EXPECTS(cond)
#  endif
#endif

#if !defined(MTP_NO_EXCEPTIONS) && defined(__EXCEPTIONS)
#  define MTP_THROW(except) throw except
#else
#  define MTP_THROW(except)
#endif

#if __has_cpp_attribute(unlikely)
#  define MTP_UNLIKELY [[unlikely]]
#else
#  define MTP_UNLIKELY
#endif

#include <version>

#ifndef MTP_BUILD_MODULE
#  include <algorithm>
#  if defined(__cpp_lib_three_way_comparison) && defined(__cpp_impl_three_way_comparison)
#    include <compare>
#  endif
#  include <cstddef>
#  include <cstdint>
#  include <cstring>
#  include <initializer_list>
#  include <iterator>
#  include <limits>
#  include <memory>
#  include <new>
#  if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
#    include <ranges>
#  endif
#  if !defined(MTP_NO_EXCEPTIONS) && defined(__EXCEPTIONS)
#    include <stdexcept>
#  endif
#  include <type_traits>
#  include <utility>
#endif

namespace mtp {

namespace detail::ipv::concepts {

template <typename R, typename T>
concept container_compatible_range =
    std::ranges::input_range<R> && std::convertible_to<std::ranges::range_reference_t<R>, T>;

} // namespace detail::ipv::concepts

namespace detail::ipv::memory {

#if __cpp_lib_raw_memory_algorithms >= 202411L
using std::uninitialized_copy;
using std::uninitialized_copy_n;
using std::uninitialized_fill;
using std::uninitialized_fill_n;
using std::uninitialized_move;
using std::uninitialized_move_n;
using std::uninitialized_value_construct;
using std::uninitialized_value_construct_n;
#else
template <std::input_iterator I, std::sentinel_for<I> S, std::forward_iterator O>
constexpr auto
uninitialized_copy(I first, S last, O d_first)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, std::iter_reference_t<I>>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = d_first;
    try {
      for (; first != last; ++current, ++first) {
        std::construct_at(std::to_address(current), *first);
      }
      return current;
    } catch (...) {
      std::destroy(d_first, current);
      throw;
    }
  }
  else {
    return std::uninitialized_copy(first, last, d_first);
  }
}

template <std::input_iterator I, typename SizeT, std::forward_iterator O>
constexpr auto
uninitialized_copy_n(I first, SizeT count, O d_first)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, std::iter_reference_t<I>>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = d_first;
    try {
      for (; count > 0; ++current, --count) {
        std::construct_at(std::to_address(current), *first);
      }
      return current;
    } catch (...) {
      std::destroy(d_first, current);
      throw;
    }
  }
  else {
    return std::uninitialized_copy_n(first, count, d_first);
  }
}

template <std::forward_iterator O, std::sentinel_for<O> S, typename T>
constexpr auto
uninitialized_fill(O first, S last, T const& value)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, T const&>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = first;
    try {
      for (; current != last; ++current) {
        std::construct_at(std::to_address(current), value);
      }
      return current;
    } catch (...) {
      std::destroy(first, current);
      throw;
    }
  }
  else {
    std::uninitialized_fill(first, last, value);
    return last;
  }
}

template <std::forward_iterator O, typename SizeT, typename T>
constexpr auto
uninitialized_fill_n(O first, SizeT count, T const& value)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, T const&>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = first;
    try {
      for (; count > 0; ++current, --count) {
        std::construct_at(std::to_address(current), value);
      }
      return current;
    } catch (...) {
      std::destroy(first, current);
      throw;
    }
  }
  else {
    return std::uninitialized_fill_n(first, count, value);
  }
}

template <std::input_iterator I, std::sentinel_for<I> S, std::forward_iterator O>
constexpr auto
uninitialized_move(I first, S last, O d_first)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, std::iter_rvalue_reference_t<I>>)
        -> O
{
  return uninitialized_copy(std::make_move_iterator(first), std::make_move_iterator(last), d_first);
}

template <std::input_iterator I, typename SizeT, std::forward_iterator O>
constexpr auto
uninitialized_move_n(I first, SizeT count, O d_first)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>, std::iter_rvalue_reference_t<I>>)
        -> O
{
  return uninitialized_copy_n(std::make_move_iterator(first), count, d_first);
}

template <std::forward_iterator O, std::sentinel_for<O> S>
constexpr auto
uninitialized_value_construct(O first, S last)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = first;
    try {
      for (; current != last; ++current) {
        std::construct_at(std::to_address(current));
      }
      return current;
    } catch (...) {
      std::destroy(first, current);
      throw;
    }
  }
  else {
    return std::uninitialized_value_construct(first, last);
  }
}

template <std::forward_iterator O, typename SizeT>
constexpr auto
uninitialized_value_construct_n(O first, SizeT count)
    noexcept(std::is_nothrow_constructible_v<std::iter_value_t<O>>) -> O
{
  if (std::is_constant_evaluated()) {
    auto current = first;
    try {
      for (; count > 0; ++current, --count) {
        std::construct_at(std::to_address(current));
      }
      return current;
    } catch (...) {
      std::destroy(first, current);
      throw;
    }
  }
  else {
    return std::uninitialized_value_construct_n(first, count);
  }
}
#endif // __cpp_lib_raw_memory_algorithms >= 202411L

MTP_EXPORT template <typename T>
struct is_trivially_relocatable : std::is_trivially_copyable<T>
{};

MTP_EXPORT template <typename T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;

MTP_EXPORT template <typename T>
struct is_nothrow_relocatable : std::bool_constant<is_trivially_relocatable_v<T> ||
                                                   (std::is_nothrow_move_constructible_v<T> &&
                                                    std::is_nothrow_destructible_v<T>)>
{};

MTP_EXPORT template <typename T>
inline constexpr bool is_nothrow_relocatable_v = is_nothrow_relocatable<T>::value;

template <typename T>
constexpr auto
relocate_at(T* dest, T* src) noexcept(is_nothrow_relocatable_v<T>) -> T*
{
  if (!std::is_constant_evaluated()) {
    if constexpr (is_trivially_relocatable_v<T>) {
      std::memmove(dest, src, sizeof(T));
      return dest;
    }
  }

  struct guard_t
  {
    T* p;
    constexpr ~guard_t() noexcept(std::is_nothrow_destructible_v<T>)
    {
      std::destroy_at(p);
    }
  } guard{ src };

  return std::construct_at(dest, std::move(*src));
}

template <std::input_iterator I, std::sentinel_for<I> S, std::forward_iterator O>
constexpr auto
uninitialized_relocate(I first, S last, O d_first)
    noexcept(is_nothrow_relocatable_v<std::iter_value_t<I>>) -> O
{
  using T = std::iter_value_t<I>;
  using U = std::iter_value_t<O>;

  if (!std::is_constant_evaluated()) {
    constexpr auto is_contiguous = std::contiguous_iterator<I> && std::contiguous_iterator<O>;
    if constexpr (is_contiguous && std::is_same_v<T, U> && is_trivially_relocatable_v<T>) {
      auto const count = static_cast<std::size_t>(last - first);
      std::memmove(std::to_address(d_first), std::to_address(first), count * sizeof(T));
      return d_first + count;
    }
  }

  auto current = d_first;
  try {
    for (; first != last; ++current, ++first) {
      relocate_at(std::to_address(current), std::to_address(first));
    }
    return current;
  } catch (...) {
    std::destroy(++first, last);
    std::destroy(d_first, current);
    throw;
  }
}

template <std::input_iterator I, typename SizeT, std::forward_iterator O>
constexpr auto
uninitialized_relocate_n(I first, SizeT count, O d_first)
    noexcept(is_nothrow_relocatable_v<std::iter_value_t<I>>) -> O
{
  MTP_EXPECTS(first && d_first);

  using T = std::iter_value_t<I>;
  using U = std::iter_value_t<O>;

  if (!std::is_constant_evaluated()) {
    constexpr auto is_contiguous = std::contiguous_iterator<I> && std::contiguous_iterator<O>;
    if constexpr (is_contiguous && std::is_same_v<T, U> && is_trivially_relocatable_v<T>) {
      std::memmove(std::to_address(d_first), std::to_address(first), count * sizeof(T));
      return d_first + count;
    }
  }

  auto current = d_first;
  try {
    for (; count > 0; ++current, ++first, --count) {
      relocate_at(std::to_address(current), std::to_address(first));
    }
    return current;
  } catch (...) {
    ++first;
    std::destroy(first, first + count);
    std::destroy(d_first, current);
    throw;
  }
}

template <std::bidirectional_iterator I, std::sentinel_for<I> S, std::bidirectional_iterator O>
constexpr auto
uninitialized_relocate_backward(I first, S last, O d_last)
    noexcept(is_nothrow_relocatable_v<std::iter_value_t<I>>) -> O
{
  using T = std::iter_value_t<I>;
  using U = std::iter_value_t<O>;

  if (!std::is_constant_evaluated()) {
    constexpr auto is_contiguous = std::contiguous_iterator<I> && std::contiguous_iterator<O>;
    if constexpr (is_contiguous && std::is_same_v<T, U> && is_trivially_relocatable_v<T>) {
      auto const count = static_cast<std::size_t>(last - first);
      std::memmove(std::to_address(d_last - count), std::to_address(first), count * sizeof(T));
      return d_last - count;
    }
  }

  auto current = d_last;
  try {
    for (; last != first;) {
      relocate_at(std::to_address(--current), std::to_address(--last));
    }
    return current;
  } catch (...) {
    std::destroy(first, ++last);
    std::destroy(++current, d_last);
    throw;
  }
}

} // namespace detail::ipv::memory
MTP_EXPORT using detail::ipv::memory::is_trivially_relocatable;
MTP_EXPORT using detail::ipv::memory::is_trivially_relocatable_v;
MTP_EXPORT using detail::ipv::memory::is_nothrow_relocatable;
MTP_EXPORT using detail::ipv::memory::is_nothrow_relocatable_v;

namespace detail::ipv::storage {

// clang-format off
template <std::size_t N>
using smallest_size_t =
    std::conditional_t<N <= std::numeric_limits<std::uint8_t>::max(),  std::uint8_t,
    std::conditional_t<N <= std::numeric_limits<std::uint16_t>::max(), std::uint16_t,
    std::conditional_t<N <= std::numeric_limits<std::uint32_t>::max(), std::uint32_t,
    std::conditional_t<N <= std::numeric_limits<std::uint64_t>::max(), std::uint64_t,
        std::size_t>>>>;
// clang-format on

template <typename T>
class zero_storage
{
public:
  using size_type = smallest_size_t<0>;

private:
  static constexpr size_type _size{ 0 };

protected:
  constexpr auto
  set_size(size_type size) noexcept -> void
  {
    MTP_EXPECTS(size <= capacity());
  }

  [[nodiscard]] constexpr auto
  size() const noexcept -> size_type
  {
    return _size;
  }

  [[nodiscard]] static constexpr auto
  capacity() noexcept -> size_type
  {
    return _size;
  }

  [[nodiscard]] constexpr auto
  data() noexcept -> T*
  {
    return static_cast<T*>(nullptr);
  }

  [[nodiscard]] constexpr auto
  data() const noexcept -> T const*
  {
    return static_cast<T const*>(nullptr);
  }
};

template <typename T, std::size_t N>
class typed_storage
{
public:
  using size_type = smallest_size_t<N>;

private:
  union {
    T _data[N];
  };
  size_type _size{ 0 };

protected:
  constexpr auto
  set_size(size_type size) noexcept -> void
  {
    MTP_EXPECTS(size <= capacity());
    _size = size;
  }

  [[nodiscard]] constexpr auto
  size() const noexcept -> size_type
  {
    return _size;
  }

  [[nodiscard]] static constexpr auto
  capacity() noexcept -> size_type
  {
    return N;
  }

  [[nodiscard]] constexpr auto
  data() noexcept -> T*
  {
    return static_cast<T*>(_data);
  }

  [[nodiscard]] constexpr auto
  data() const noexcept -> T const*
  {
    return static_cast<T const*>(_data);
  }
};

template <typename T, std::size_t N>
class byte_storage
{
public:
  using size_type = smallest_size_t<N>;

private:
  alignas(T) std::byte _data[N * sizeof(T)];
  size_type _size{ 0 };

protected:
  constexpr auto
  set_size(size_type size) noexcept -> void
  {
    MTP_EXPECTS(size <= capacity());
    _size = size;
  }

  [[nodiscard]] constexpr auto
  size() const noexcept -> size_type
  {
    return _size;
  }

  [[nodiscard]] static constexpr auto
  capacity() noexcept -> size_type
  {
    return N;
  }

  [[nodiscard]] constexpr auto
  data() noexcept -> T*
  {
    return reinterpret_cast<T*>(_data);
  }

  [[nodiscard]] constexpr auto
  data() const noexcept -> T const*
  {
    return reinterpret_cast<T const*>(_data);
  }
};

// clang-format off
template <typename T, std::size_t N>
using storage_type = std::conditional_t<
    N == 0,
    zero_storage<T>,
    std::conditional_t<
        std::is_trivially_default_constructible_v<T> && std::is_trivially_destructible_v<T>,
        typed_storage<T, N>,
        byte_storage<T, N>
    >
>;
// clang-format on

} // namespace detail::ipv::storage

MTP_EXPORT template <typename T, std::size_t N>
class inplace_vector : private detail::ipv::storage::storage_type<T, N>
{
public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = T const*;
  using reference = T&;
  using const_reference = T const&;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

private:
  using _storage = detail::ipv::storage::storage_type<T, N>;

  constexpr auto
  _unsafe_set_size(size_type size) noexcept -> void
  {
    _storage::set_size(static_cast<_storage::size_type>(size));
  }

  [[nodiscard]] constexpr auto
  _is_valid_iterator(const_iterator pos) const noexcept -> bool
  {
    return begin() <= pos && pos <= end();
  }

  [[nodiscard]] constexpr auto
  _is_valid_iterator_pair(const_iterator first, const_iterator last) const noexcept -> bool
  {
    return _is_valid_iterator(first) && _is_valid_iterator(last) && first <= last;
  }

public:
  inplace_vector() = default;

  inplace_vector(inplace_vector const&)
    requires(N == 0 || std::is_trivially_copy_constructible_v<value_type>)
  = default;

  inplace_vector& operator=(inplace_vector const&)
    requires(N == 0 || (std::is_trivially_copy_constructible_v<value_type> &&
                        std::is_trivially_copy_assignable_v<value_type> &&
                        std::is_trivially_destructible_v<value_type>))
  = default;

  inplace_vector(inplace_vector&&)
    requires(N == 0 || std::is_trivially_move_constructible_v<value_type>)
  = default;

  inplace_vector& operator=(inplace_vector&&)
    requires(N == 0 || (std::is_trivially_move_constructible_v<value_type> &&
                        std::is_trivially_move_assignable_v<value_type> &&
                        std::is_trivially_destructible_v<value_type>))
  = default;

  ~inplace_vector()
    requires(N == 0 || std::is_trivially_destructible_v<value_type>)
  = default;

  constexpr inplace_vector(inplace_vector const& ipv)
      noexcept(std::is_nothrow_copy_constructible_v<value_type>)
  {
    insert(data(), ipv.begin(), ipv.end());
  }

  constexpr auto operator=(inplace_vector const& ipv)
      noexcept(std::is_nothrow_copy_constructible_v<value_type> &&
               std::is_nothrow_copy_assignable_v<value_type> &&
               std::is_nothrow_destructible_v<value_type>) -> inplace_vector&
  {
    assign(ipv.begin(), ipv.end());
    return *this;
  }

  constexpr inplace_vector(inplace_vector&& ipv)
      noexcept(is_trivially_relocatable_v<value_type> ||
               std::is_nothrow_move_constructible_v<value_type>)
  {
    if constexpr (is_trivially_relocatable_v<value_type>) {
      using detail::ipv::memory::uninitialized_relocate;
      uninitialized_relocate(ipv.begin(), ipv.end(), data());
      _unsafe_set_size(ipv.size());
      ipv._unsafe_set_size(0);
    }
    else {
      using detail::ipv::memory::uninitialized_move;
      uninitialized_move(ipv.begin(), ipv.end(), data());
      _unsafe_set_size(ipv.size());
    }
  }

  constexpr auto operator=(inplace_vector&& ipv)
      noexcept(is_trivially_relocatable_v<value_type> ||
               (std::is_nothrow_move_constructible_v<value_type> &&
                std::is_nothrow_move_assignable_v<value_type> &&
                std::is_nothrow_destructible_v<value_type>)) -> inplace_vector&
  {
    if (size() <= ipv.size()) {
      auto it = std::move(ipv.begin(), ipv.begin() + size(), data());
      if constexpr (is_trivially_relocatable_v<value_type>) {
        using detail::ipv::memory::uninitialized_relocate;
        uninitialized_relocate(ipv.begin() + size(), ipv.end(), it);
      }
      else {
        using detail::ipv::memory::uninitialized_move;
        uninitialized_move(ipv.begin() + size(), ipv.end(), it);
      }
    }
    else {
      auto it = std::move(ipv.begin(), ipv.end(), data());
      std::destroy(it, data() + size());
    }

    _unsafe_set_size(ipv.size());
    ipv._unsafe_set_size(0);
    return *this;
  }

  constexpr ~inplace_vector() noexcept(std::is_nothrow_destructible_v<value_type>)
  {
    clear();
  }

  explicit constexpr inplace_vector(size_type count)
  {
    resize(count);
  }

  constexpr inplace_vector(size_type count, value_type const& value)
  {
    resize(count, value);
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr inplace_vector(I first, S last)
  {
    insert(data(), first, last);
  }

#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
  template <detail::ipv::concepts::container_compatible_range<value_type> R>
  constexpr inplace_vector(std::from_range_t, R&& rg)
  {
    insert_range(data(), std::forward<R>(rg));
  }
#endif

  constexpr inplace_vector(std::initializer_list<value_type> ilist)
  {
    insert(data(), ilist);
  }

  constexpr auto operator=(std::initializer_list<value_type> ilist) -> inplace_vector&
  {
    assign(ilist);
    return *this;
  }

  constexpr auto
  assign(size_type count, value_type const& value) -> void
  {
    if (count <= size()) {
      auto const it = std::fill_n(data(), count, value);
      std::destroy(it, data() + size());
    }
    else if (count <= capacity()) {
      using detail::ipv::memory::uninitialized_fill_n;
      auto const it = std::fill_n(data(), size(), value);
      uninitialized_fill_n(it, count - size(), value);
    }
    else
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }

    _unsafe_set_size(count);
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr auto assign(I first, S last) -> void
  {
    auto it = data();
    auto const old_end = data() + size();
    for (; first != last && it != old_end; ++it, ++first) {
      *it = *first;
    }

    if (first == last) {
      std::destroy(it, old_end);
    }
    else {
      for (; first != last; ++first) {
        emplace_back(*first);
      }
    }
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
    requires(std::random_access_iterator<I>)
  constexpr auto assign(I first, S last) -> void
  {
    auto const count = static_cast<size_type>(last - first);
    if (count > capacity())
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }

    auto it = data();
    auto const old_end = data() + size();
    for (; first != last && it != old_end; ++it, ++first) {
      *it = *first;
    }

    if (first == last) {
      std::destroy(it, old_end);
    }
    else {
      using detail::ipv::memory::uninitialized_copy;
      uninitialized_copy(first, last, it);
      _unsafe_set_size(count);
    }
  }

  template <detail::ipv::concepts::container_compatible_range<value_type> R>
  constexpr auto assign_range(R&& rg)
  {
    assign(std::ranges::begin(rg), std::ranges::end(rg));
  }

  constexpr auto assign(std::initializer_list<value_type> ilist) -> void
  {
    assign(ilist.begin(), ilist.end());
  }

  [[nodiscard]] constexpr auto
  size() const noexcept -> size_type
  {
    return _storage::size();
  }

  [[nodiscard]] static constexpr auto
  max_size() noexcept -> size_type
  {
    return _storage::capacity();
  }

  [[nodiscard]] static constexpr auto
  capacity() noexcept -> size_type
  {
    return _storage::capacity();
  }

  [[nodiscard]] constexpr auto
  empty() const noexcept -> bool
  {
    if constexpr (capacity() == 0) {
      return true;
    }
    else {
      return size() == 0;
    }
  }

  static constexpr auto
  reserve(size_type new_cap) -> void
  {
    if (new_cap > capacity())
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }
  }

  static constexpr auto
  shrink_to_fit() noexcept -> void
  {}

  constexpr auto
  resize(size_type count) -> void
  {
    if (count < size()) {
      std::destroy(data() + count, data() + size());
      _unsafe_set_size(count);
    }
    else if (count == size()) {
      return;
    }
    else if (count <= capacity()) {
      using detail::ipv::memory::uninitialized_value_construct_n;
      uninitialized_value_construct_n(data() + size(), count - size());
      _unsafe_set_size(count);
    }
    else
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }
  }

  constexpr auto
  resize(size_type count, value_type const& value) -> void
  {
    if (count < size()) {
      std::destroy(data() + count, data() + size());
      _unsafe_set_size(count);
    }
    else if (count == size()) {
      return;
    }
    else if (count <= capacity()) {
      insert(data() + size(), count - size(), value);
    }
    else
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }
  }

  [[nodiscard]] constexpr auto
  at(size_type pos) -> reference
  {
    if (pos >= size())
      MTP_UNLIKELY
      {
        MTP_THROW(std::out_of_range("mtp::inplace_vector::at"));
      }
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  at(size_type pos) const -> const_reference
  {
    if (pos >= size())
      MTP_UNLIKELY
      {
        MTP_THROW(std::out_of_range("mtp::inplace_vector::at"));
      }
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  operator[](size_type pos) -> reference
  {
    MTP_EXPECTS(pos < size());
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  operator[](size_type pos) const -> const_reference
  {
    MTP_EXPECTS(pos < size());
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  front() -> reference
  {
    MTP_EXPECTS(!empty());
    return data()[0];
  }

  [[nodiscard]] constexpr auto
  front() const -> const_reference
  {
    MTP_EXPECTS(!empty());
    return data()[0];
  }

  [[nodiscard]] constexpr auto
  back() -> reference
  {
    MTP_EXPECTS(!empty());
    return data()[size() - 1];
  }

  [[nodiscard]] constexpr auto
  back() const -> const_reference
  {
    MTP_EXPECTS(!empty());
    return data()[size() - 1];
  }

  [[nodiscard]] constexpr auto
  data() noexcept -> pointer
  {
    return _storage::data();
  }

  [[nodiscard]] constexpr auto
  data() const noexcept -> const_pointer
  {
    return _storage::data();
  }

  [[nodiscard]] constexpr auto
  begin() noexcept -> iterator
  {
    return data();
  }

  [[nodiscard]] constexpr auto
  end() noexcept -> iterator
  {
    return data() + size();
  }

  [[nodiscard]] constexpr auto
  begin() const noexcept -> const_iterator
  {
    return data();
  }

  [[nodiscard]] constexpr auto
  end() const noexcept -> const_iterator
  {
    return data() + size();
  }

  [[nodiscard]] constexpr auto
  cbegin() const noexcept -> const_iterator
  {
    return data();
  }

  [[nodiscard]] constexpr auto
  cend() const noexcept -> const_iterator
  {
    return data() + size();
  }

  [[nodiscard]] constexpr auto
  rbegin() noexcept -> reverse_iterator
  {
    return reverse_iterator{ end() };
  }

  [[nodiscard]] constexpr auto
  rend() noexcept -> reverse_iterator
  {
    return reverse_iterator{ begin() };
  }

  [[nodiscard]] constexpr auto
  rbegin() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ end() };
  }

  [[nodiscard]] constexpr auto
  rend() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ begin() };
  }

  [[nodiscard]] constexpr auto
  crbegin() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ end() };
  }

  [[nodiscard]] constexpr auto
  crend() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ begin() };
  }

  template <typename... Args>
  constexpr auto
  emplace_back(Args&&... args) -> reference
  {
    auto const it = try_emplace_back(std::forward<Args>(args)...);
    if (!it)
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }
    return *it;
  }

  constexpr auto
  push_back(value_type const& value) -> reference
  {
    return emplace_back(value);
  }

  constexpr auto
  push_back(value_type&& value) -> reference
  {
    return emplace_back(std::forward<value_type>(value));
  }

  template <detail::ipv::concepts::container_compatible_range<value_type> R>
  constexpr auto append_range(R&& rg) -> void
  {
    auto const it = try_append_range(std::forward<R>(rg));
    if (it != std::ranges::end(rg))
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }
  }

  constexpr auto
  pop_back() -> void
  {
    MTP_EXPECTS(!empty());
    std::destroy_at(data() + size() - 1);
    _unsafe_set_size(size() - 1);
  }

  template <typename... Args>
  constexpr auto
  try_emplace_back(Args&&... args) -> pointer
  {
    if (size() >= capacity())
      MTP_UNLIKELY
      {
        return nullptr;
      }
    return std::addressof(unchecked_emplace_back(std::forward<Args>(args)...));
  }

  constexpr auto
  try_push_back(value_type const& value) -> pointer
  {
    return try_emplace_back(value);
  }

  constexpr auto
  try_push_back(value_type&& value) -> pointer
  {
    return try_emplace_back(std::forward<value_type>(value));
  }

  template <detail::ipv::concepts::container_compatible_range<value_type> R>
  constexpr auto
  try_append_range(R&& rg) -> std::ranges::borrowed_iterator_t<R>
  {
    auto first = std::ranges::begin(rg);
    for (; first != std::ranges::end(rg); ++first) {
      if (!try_emplace_back(*first)) {
        return first;
      }
    }
    return first;
  }

  template <detail::ipv::concepts::container_compatible_range<value_type> R>
    requires(std::ranges::sized_range<R>)
  constexpr auto
  try_append_range(R&& rg) -> std::ranges::borrowed_iterator_t<R>
  {
    auto const count = std::min<size_type>(capacity() - size(), std::ranges::size(rg));
    auto const first = std::ranges::begin(rg);

    using detail::ipv::memory::uninitialized_copy;
    uninitialized_copy(first, first + count, data() + size());
    _unsafe_set_size(size() + count);

    return first + count;
  }

  template <typename... Args>
  constexpr auto
  unchecked_emplace_back(Args&&... args) -> reference
  {
    MTP_EXPECTS(size() < capacity());
    auto const it = std::construct_at(data() + size(), std::forward<Args>(args)...);
    _unsafe_set_size(size() + 1);
    return *it;
  }

  constexpr auto
  unchecked_push_back(value_type const& value) -> reference
  {
    return unchecked_emplace_back(value);
  }

  constexpr auto
  unchecked_push_back(value_type&& value) -> reference
  {
    return unchecked_emplace_back(std::forward<value_type>(value));
  }

  template <typename... Args>
  constexpr auto
  emplace(const_iterator pos, Args&&... args) -> iterator
  {
    MTP_EXPECTS(_is_valid_iterator(pos));
    if (size() >= capacity())
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }

    auto const it = iterator(pos);
    auto const old_end = data() + size();

    if constexpr (is_trivially_relocatable_v<value_type>) {
      using detail::ipv::memory::uninitialized_relocate_backward;
      uninitialized_relocate_backward(it, old_end, old_end + 1);
      try {
        std::construct_at(it, std::forward<Args>(args)...);
        _unsafe_set_size(size() + 1);
      } catch (...) {
        using detail::ipv::memory::uninitialized_relocate;
        uninitialized_relocate(it + 1, old_end + 1, it);
        throw;
      }
    }
    else {
      unchecked_emplace_back(std::forward<Args>(args)...);
      std::rotate(it, old_end, old_end + 1);
    }

    return it;
  }

  constexpr auto
  insert(const_iterator pos, value_type const& value) -> iterator
  {
    return emplace(pos, value);
  }

  constexpr auto
  insert(const_iterator pos, value_type&& value) -> iterator
  {
    return emplace(pos, std::forward<value_type>(value));
  }

  constexpr auto
  insert(const_iterator pos, size_type count, value_type const& value) -> iterator
  {
    MTP_EXPECTS(_is_valid_iterator(pos));
    if (count + size() > capacity() )
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }

    auto const it = iterator(pos);
    auto const old_end = data() + size();

    if constexpr (is_trivially_relocatable_v<value_type>) {
      using detail::ipv::memory::uninitialized_relocate_backward;
      uninitialized_relocate_backward(it, old_end, old_end + count);
      try {
        using detail::ipv::memory::uninitialized_fill_n;
        uninitialized_fill_n(it, count, value);
      } catch (...) {
        using detail::ipv::memory::uninitialized_relocate;
        uninitialized_relocate(it + count, old_end + count, it);
        throw;
      }
    }
    else {
      using detail::ipv::memory::uninitialized_fill_n;
      uninitialized_fill_n(old_end, count, value);
      std::rotate(it, old_end, old_end + count);
    }
    _unsafe_set_size(size() + count);

    return it;
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr auto
  insert(const_iterator pos, I first, S last) -> iterator
  {
    MTP_EXPECTS(_is_valid_iterator(pos));

    auto const it = iterator(pos);
    auto const old_end = data() + size();

    for (; first != last; ++first) {
      emplace_back(*first);
    }
    auto const new_end = data() + size();
    std::rotate(it, old_end, new_end);

    return it;
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
    requires(std::random_access_iterator<I>)
  constexpr auto
  insert(const_iterator pos, I first, S last) -> iterator
  {
    MTP_EXPECTS(_is_valid_iterator(pos));
    auto const count = static_cast<size_type>(last - first);
    if (count + size() > capacity())
      MTP_UNLIKELY
      {
        MTP_THROW(std::bad_alloc());
      }

    auto const it = iterator(pos);
    auto const old_end = data() + size();

    if constexpr (is_trivially_relocatable_v<value_type>) {
      using detail::ipv::memory::uninitialized_relocate_backward;
      uninitialized_relocate_backward(it, old_end, old_end + count);
      try {
        using detail::ipv::memory::uninitialized_copy;
        uninitialized_copy(first, last, it);
      } catch (...) {
        using detail::ipv::memory::uninitialized_relocate;
        uninitialized_relocate(it + count, old_end + count, it);
        throw;
      }
    }
    else {
      using detail::ipv::memory::uninitialized_copy;
      uninitialized_copy(first, last, old_end);
      std::rotate(it, old_end, old_end + count);
    }
    _unsafe_set_size(size() + count);

    return it;
  }

  template <detail::ipv::concepts::container_compatible_range<value_type> R>
  constexpr auto
  insert_range(const_iterator pos, R&& rg) -> iterator
  {
    return insert(pos, std::ranges::begin(rg), std::ranges::end(rg));
  }

  constexpr auto
  insert(const_iterator pos, std::initializer_list<value_type> ilist) -> iterator
  {
    return insert(pos, ilist.begin(), ilist.end());
  }

  constexpr auto
  erase(const_iterator pos) -> iterator
  {
    erase(pos, pos + 1);
  }

  constexpr auto
  erase(const_iterator first, const_iterator last) -> iterator
  {
    MTP_EXPECTS(_is_valid_iterator_pair(first, last));

    auto const it = iterator(first);
    auto const old_end = data() + size();
    auto const count = static_cast<size_type>(last - first);

    if constexpr (is_trivially_relocatable_v<value_type>) {
      using detail::ipv::memory::uninitialized_relocate;
      std::destroy(it, it + count);
      uninitialized_relocate(it + count, old_end, it);
    }
    else {
      std::destroy(std::move(it + count, old_end, it), old_end);
    }
    _unsafe_set_size(size() - count);

    return it;
  }

  constexpr auto
  clear() noexcept(std::is_nothrow_destructible_v<value_type>) -> void
  {
    erase(data(), data() + size());
  }

  constexpr auto swap(inplace_vector& ipv)
      noexcept(N == 0 ||
               (std::is_nothrow_swappable_v<T> && std::is_nothrow_move_constructible_v<T>)) -> void
  {
    if (this == std::addressof(ipv)) MTP_UNLIKELY {
      return;
    }

    if (size() < ipv.size()) {
      ipv.swap(*this);
    }
    else {
      auto const ipv_it = std::swap_ranges(data(), data() + ipv.size(), ipv.data());
      if constexpr (is_trivially_relocatable_v<value_type>) {
        using detail::ipv::memory::uninitialized_relocate;
        uninitialized_relocate(data() + ipv.size(), data() + size(), ipv_it);
      }
      else {
        using detail::ipv::memory::uninitialized_move;
        uninitialized_move(data() + ipv.size(), data() + size(), ipv_it);
        std::destroy(data() + ipv.size(), data() + size());
      }
      auto const old_size = size();
      _unsafe_set_size(ipv.size());
      ipv._unsafe_set_size(old_size);
    }
  }

  [[nodiscard]] friend constexpr auto
  operator==(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

#if defined(__cpp_lib_three_way_comparison) && defined(__cpp_impl_three_way_comparison)
  [[nodiscard]] friend constexpr auto
  operator<=>(inplace_vector const& lhs, inplace_vector const& rhs) noexcept
  {
    auto it_lhs = lhs.begin();
    auto it_rhs = rhs.begin();
    for (; it_lhs != lhs.end() && it_rhs != rhs.end(); ++it_lhs, ++it_rhs) {
      if (auto const cmp = *it_lhs <=> *it_rhs; cmp != 0) {
        return cmp;
      }
    }
    return lhs.size() <=> rhs.size();
  }
#else
  [[nodiscard]] friend constexpr auto
  operator<(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    auto it_lhs = lhs.begin();
    auto it_rhs = rhs.begin();
    for (; it_lhs != lhs.end() && it_rhs != rhs.end(); ++it_lhs, ++it_rhs) {
      if (*it_lhs < *it_rhs) {
        return true;
      }
      else if (*it_lhs > *it_rhs) {
        return false;
      }
    }
    return lhs.size() < rhs.size();
  }

  [[nodiscard]] friend constexpr auto
  operator>(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    return rhs < lhs;
  }

  [[nodiscard]] friend constexpr auto
  operator<=(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    return !(rhs < lhs);
  }

  [[nodiscard]] friend constexpr auto
  operator>=(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    return !(lhs < rhs);
  }

  [[nodiscard]] friend constexpr auto
  operator!=(inplace_vector const& lhs, inplace_vector const& rhs) noexcept -> bool
  {
    return !(lhs == rhs);
  }
#endif // __cpp_lib_three_way_comparison && __cpp_impl_three_way_comparison

  friend constexpr auto
  swap(inplace_vector& a, inplace_vector& b)
      noexcept(N == 0 ||
               (std::is_nothrow_swappable_v<T> && std::is_nothrow_move_constructible_v<T>)) -> void
  {
    return a.swap(b);
  }
};

template <typename T, std::size_t N>
struct is_trivially_relocatable<inplace_vector<T, N>>
    : std::bool_constant<N == 0 || is_trivially_relocatable_v<T>>
{};

} // namespace mtp

#undef MTP_EXPORT
#undef MTP_EXPECTS
#undef MTP_THROW
#undef MTP_UNLIKELY

#endif // MTP_INPLACE_VECTOR_HPP
