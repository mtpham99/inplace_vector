#include <catch2/catch.hpp>

#include <version>

#ifdef MTP_USE_STD_MODULE
import std;
#else
#  include <algorithm>
#  if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
#    include <ranges>
#  endif
#  include <type_traits>
#endif

#ifdef MTP_BUILD_MODULE
import mtp.inplace_vector;
#else
#  include <mtp/inplace_vector.hpp>
#endif

namespace {

using mtp::inplace_vector;

struct trivial
{
  int value;

  constexpr trivial(int v) : value{ v } {}
  constexpr
  operator int() const noexcept
  {
    return value;
  }

  trivial() = default;
};
static_assert(std::is_trivially_default_constructible_v<trivial>);
static_assert(std::is_trivially_copy_constructible_v<trivial>);
static_assert(std::is_trivially_copy_assignable_v<trivial>);
static_assert(std::is_trivially_move_constructible_v<trivial>);
static_assert(std::is_trivially_move_assignable_v<trivial>);
static_assert(std::is_trivially_destructible_v<trivial>);

struct non_trivial
{
  int value;

  constexpr non_trivial(int v) : value{ v } {}
  constexpr
  operator int() const noexcept
  {
    return value;
  }

  non_trivial() = default;
  ~non_trivial() {} // non-trivial
};
static_assert(!std::is_trivially_default_constructible_v<non_trivial> &&
              std::is_default_constructible_v<non_trivial>);
static_assert(!std::is_trivially_copy_constructible_v<non_trivial> &&
              std::is_copy_constructible_v<non_trivial>);
static_assert(std::is_trivially_copy_assignable_v<non_trivial>);
static_assert(!std::is_trivially_move_constructible_v<non_trivial> &&
              std::is_move_constructible_v<non_trivial>);
static_assert(std::is_trivially_move_assignable_v<non_trivial>);
static_assert(!std::is_trivially_destructible_v<non_trivial> &&
              std::is_destructible_v<non_trivial>);

struct move_only
{
  int value;

  constexpr move_only(int v) : value{ v } {}
  constexpr
  operator int() const noexcept
  {
    return value;
  }

  move_only() = default;
  move_only(move_only const&) = delete;
  move_only& operator=(move_only const&) = delete;
  move_only(move_only&&) = default;
  move_only& operator=(move_only&&) = default;
};
static_assert(std::is_trivially_default_constructible_v<move_only>);
static_assert(!std::is_copy_constructible_v<move_only>);
static_assert(!std::is_copy_assignable_v<move_only>);
static_assert(std::is_trivially_move_constructible_v<move_only>);
static_assert(std::is_trivially_move_assignable_v<move_only>);
static_assert(std::is_trivially_destructible_v<move_only>);

template <typename T, std::size_t N>
consteval auto
test_triviality() -> void
{
  using IpvT = inplace_vector<T, N>;

  if constexpr (N == 0) {
    static_assert(std::is_empty_v<IpvT> == true && sizeof(IpvT) == 1);

    static_assert(std::is_trivially_copyable_v<IpvT>);
    static_assert(mtp::is_trivially_relocatable_v<IpvT>);

    static_assert(std::is_trivially_default_constructible_v<IpvT>);
    static_assert(std::is_trivially_copy_constructible_v<IpvT>);
    static_assert(std::is_trivially_copy_assignable_v<IpvT>);
    static_assert(std::is_trivially_move_constructible_v<IpvT>);
    static_assert(std::is_trivially_move_assignable_v<IpvT>);
    static_assert(std::is_trivially_destructible_v<IpvT>);

    static_assert(std::is_nothrow_default_constructible_v<IpvT>);
    static_assert(std::is_nothrow_copy_constructible_v<IpvT>);
    static_assert(std::is_nothrow_copy_assignable_v<IpvT>);
    static_assert(std::is_nothrow_move_constructible_v<IpvT>);
    static_assert(std::is_nothrow_move_assignable_v<IpvT>);
    static_assert(std::is_nothrow_destructible_v<IpvT>);
  }
  else {
    static_assert(!std::is_empty_v<IpvT>);

    // trivially copyable bugs
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96288
    // https://github.com/llvm/llvm-project/issues/38398
    // static_assert(!std::is_trivially_copyable_v<T> || std::is_trivially_copyable_v<IpvT>);
    static_assert(!mtp::is_trivially_relocatable_v<T> || mtp::is_trivially_relocatable_v<IpvT>);

    static_assert(std::is_default_constructible_v<IpvT>);
    static_assert(!std::is_trivially_default_constructible_v<IpvT>);

    static_assert(!std::is_copy_constructible_v<T> || std::is_copy_constructible_v<IpvT>);
    static_assert(!std::is_trivially_copy_constructible_v<T> ||
                  std::is_trivially_copy_constructible_v<IpvT>);

    static_assert(!(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>) ||
                  std::is_copy_assignable_v<IpvT>);
    static_assert(
        !(std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_assignable_v<T>) ||
        std::is_trivially_copy_assignable_v<IpvT>);

    static_assert(!std::is_move_constructible_v<T> || std::is_move_constructible_v<IpvT>);
    static_assert(!std::is_trivially_move_constructible_v<T> ||
                  std::is_trivially_move_constructible_v<IpvT>);

    static_assert(!(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>) ||
                  std::is_move_assignable_v<IpvT>);
    static_assert(
        !(std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T>) ||
        std::is_trivially_move_assignable_v<IpvT>);

    static_assert(!std::is_nothrow_default_constructible_v<T> ||
                  std::is_nothrow_default_constructible_v<IpvT>);
    static_assert(!std::is_nothrow_copy_constructible_v<T> ||
                  std::is_nothrow_copy_constructible_v<IpvT>);
    static_assert(
        !(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>) ||
        std::is_nothrow_copy_assignable_v<IpvT>);
    static_assert(!std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<IpvT>);
    static_assert(
        !(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>) ||
        std::is_nothrow_move_assignable_v<IpvT>);
    static_assert(!std::is_nothrow_destructible_v<T> || std::is_nothrow_destructible_v<IpvT>);
  }
}

template <typename T>
auto
test_zero_sized() -> void
{
  using IpvT = inplace_vector<T, 0>;
  auto ipv = IpvT{};

  SECTION("size and capacity")
  {
    static_assert(ipv.max_size() == 0 && ipv.capacity() == 0);
    CHECK([&]() { return ipv.size() == 0 && ipv.empty(); }());
  }

  SECTION("access")
  {
    CHECK(ipv.data() == nullptr);
    CHECK_THROWS_AS(ipv.at(0), std::out_of_range);
  }

  SECTION("modify")
  {
    auto const val = T{};
    auto const arr = std::array<T, 1>{ T{} };
    auto const ilist = std::initializer_list<T>{ T{} };
    CHECK_THROWS_AS(ipv.emplace_back(val), std::bad_alloc);
    CHECK(ipv.try_emplace_back(val) == nullptr);
    CHECK_THROWS_AS(ipv.push_back(val), std::bad_alloc);
    CHECK(ipv.try_push_back(val) == nullptr);
    CHECK_THROWS_AS(ipv.append_range(arr), std::bad_alloc);
    CHECK(ipv.try_append_range(arr) == arr.begin());
    CHECK_THROWS_AS(ipv.emplace(ipv.end(), val), std::bad_alloc);
    CHECK_THROWS_AS(ipv.insert(ipv.end(), val), std::bad_alloc);
    CHECK_THROWS_AS(ipv.insert(ipv.end(), 1, val), std::bad_alloc);
    CHECK_THROWS_AS(ipv.insert(ipv.end(), arr.begin(), arr.end()), std::bad_alloc);
    CHECK_THROWS_AS(ipv.insert_range(ipv.end(), arr), std::bad_alloc);
    CHECK_THROWS_AS(ipv.insert(ipv.end(), ilist), std::bad_alloc);
  }
}

template <typename T, std::size_t N>
  requires(N > 0)
auto
test_constructors() -> void
{
  using IpvT = inplace_vector<T, N>;

  auto const vec_halfN = []() {
    auto v = std::vector<T>{};
    v.reserve(N / 2);
    for (auto i = 0u; i < N / 2; ++i) {
      v.emplace_back(i);
    }
    return v;
  }();

  SECTION("default")
  {
    auto const ipv = IpvT{};
    CHECK(ipv.size() == 0);
  }

  SECTION("size")
  {
    { // default
      auto const ipv = IpvT(N / 2);
      auto const vec = std::vector<T>(N / 2);
      CHECK(ipv.size() == N / 2);
      CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));
    }
    { // value
      auto const val = T{ 1 };
      auto const ipv = IpvT(N, val);
      auto const vec = std::vector<T>(N, val);
      CHECK(ipv.size() == N);
      CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));
    }
  }

  SECTION("iterators")
  {
    auto const ipv = IpvT{vec_halfN.begin(), vec_halfN.end()};
    CHECK(ipv.size() == vec_halfN.size());
    CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
  }


#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
  SECTION("range")
  {
    auto const ipv = IpvT(std::from_range, vec_halfN);
    CHECK(ipv.size() == vec_halfN.size());
    CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
  }
#endif

  SECTION("copy")
  {
    auto const tmp = IpvT(vec_halfN.begin(), vec_halfN.end());

    { // construct
      auto const ipv = IpvT{tmp};
      CHECK(ipv.size() == vec_halfN.size());
      CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
    }
    { // assignment
      auto ipv = IpvT{};
      ipv = tmp;
      CHECK(ipv.size() == vec_halfN.size());
      CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
    }
  }

  SECTION("move")
  {
    { // construct
      auto tmp = IpvT(vec_halfN.begin(), vec_halfN.end());
      auto const ipv = IpvT{std::move(tmp)};
      CHECK(ipv.size() == vec_halfN.size());
      CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
    }
    { // assignment
      auto tmp = IpvT(vec_halfN.begin(), vec_halfN.end());
      auto ipv = IpvT{};
      ipv = std::move(tmp);
      CHECK(ipv.size() == vec_halfN.size());
      CHECK(std::equal(vec_halfN.begin(), vec_halfN.end(), ipv.begin()));
    }
  }
}

template <typename T>
auto
test_modifications() -> void
{
  constexpr auto N = 8;
  using IpvT = inplace_vector<T, N>;

  auto ipv = IpvT{};
  auto vec = std::vector<T>{};
  vec.reserve(N);
  CHECK(ipv.size() == vec.size());
  CHECK(ipv.capacity() == vec.capacity());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // push_back
  ipv.push_back(T{0});
  vec.push_back(T{0});
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // emplace_back
  ipv.emplace_back(T{1});
  vec.emplace_back(T{1});
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // insert
  auto ipv_it = ipv.end();
  auto vec_it = vec.end();
  auto const arr_34 = std::array<T, 2>{ T{3}, T{4} };
  ipv.insert(ipv_it, arr_34.begin(), arr_34.end());
  vec.insert(vec_it, arr_34.begin(), arr_34.end());
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // emplace
  ipv.emplace(ipv_it, 2);
  vec.emplace(vec_it, 2);
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // erase
  ipv_it = ipv.begin() + 2;
  vec_it = vec.begin() + 2;
  ipv.erase(ipv_it, ipv_it + 1);
  vec.erase(vec_it, vec_it + 1);
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // clear
  auto ipv_copy = ipv;
  auto vec_copy = vec;
  ipv.clear();
  vec.clear();
  CHECK(ipv == IpvT{});
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));

  // swap
  ipv.swap(ipv_copy);
  vec.swap(vec_copy);
  CHECK(ipv.size() == vec.size());
  CHECK(std::equal(vec.begin(), vec.end(), ipv.begin()));
}

} // namespace

TEMPLATE_TEST_CASE("triviality", "[inplace_vector]", trivial, non_trivial, move_only)
{
  using T = TestType;
  test_triviality<T, 0>();
  test_triviality<T, 1>();
}

TEMPLATE_TEST_CASE("zero sized", "[inplace_vector]", trivial, non_trivial)
{
  using T = TestType;
  test_zero_sized<T>();
}

TEMPLATE_TEST_CASE("constructors", "[inplace_vector]", trivial, non_trivial)
{
  using T = TestType;
  test_constructors<T, 2>();
  test_constructors<T, 4>();
  test_constructors<T, 8>();
}

TEMPLATE_TEST_CASE("modifications", "[inplace_vector]", trivial, non_trivial)
{
  using T = TestType;
  test_modifications<T>();
}

TEMPLATE_TEST_CASE("constexpr support", "[inplace_vector]", trivial)
{
  using T = TestType;
  using IpvT = inplace_vector<T, 4>;

  static_assert(std::is_trivially_default_constructible_v<T> && std::is_trivially_destructible_v<T>);

  constexpr auto ipv = []() {
    auto v = IpvT{};
    v.push_back(T{1});
    v.push_back(T{2});
    return v;
  }();
  static_assert(ipv.size() == 2 && ipv.front() == T{1} && ipv.back() == T{2});
}
