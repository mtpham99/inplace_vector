# inplace_vector (mtp-libs)

A c++20 implementation of [proposal 0843: inplace_vector](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0843r14.html) with [proposal 1144: trivial relocation](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p1144r12.html) related optimizations.

Available as both single header and [c++20 module](https://en.cppreference.com/w/cpp/language/modules).


# Example

See [tests](/test/inplace_vector_test.cpp) for more examples.

```cpp
import std;

import mtp::inplace_vector;
using mtp::inplace_vector;

template <std::size_t N>
  requires(N >= 2)
consteval auto
make_fib() noexcept -> inplace_vector<int, N> {
  auto fib = inplace_vector<int, N>{};
  fib.push_back(0); fib.push_back(1);
  for (auto i = 2u; i < N; ++i) {
    fib.push_back(fib[i - 1] + fib[i - 2]);
  }
  return fib;
}

auto main() -> int {
  constexpr auto n = 8;
  constexpr auto fib = make_fib<n>();
  constexpr auto check = std::array{ 0, 1, 1, 2, 3, 5, 8, 13 };
  static_assert(fib.size() == check.size());
  static_assert(std::equal(check.begin(), check.end(), fib.begin()));
  return 0;
}
```


# Build

## Single header

Drop [inplace_vector.hpp](/include/mtp/inplace_vector.hpp) directly in your project where your compiler can find it.

Exceptions can be disabled using a macro prior to including the header:

```cpp
#define MTP_NO_EXCEPTIONS
#include <mtp/inplace_vector.hpp>
```


## CMake

Add subdirectory and link against `mtp::inplace_vector`.

```cmake
add_subdirectory(PATH_TO_THIS_DIR)
target_link_libraries(MY_TARGET mtp::inplace_vector)
```

Then include/import as usual:

```cpp
#include <mtp/inplace_vector.hpp> // header-only
import mtp.inplace_vector;        // module
```

Optional build options:

1. `MTP_BUILD_TEST`: build tests (default: on if this is the top level project)
2. `MTP_NO_EXCEPTIONS`: disable exceptions (default: off)
3. `MTP_BUILD_MODULE`: build as module instead of header-only (default: off)
4. `MTP_USE_STD_MODULE`: use [c++23 std module](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) (default: off)

Example module build (requires CMake 3.30+, Ninja 1.11+, Clang/Libc++ 18.1.2+):

```sh
cmake -S . -B build -G Ninja -DCMAKE_CXX_FLAGS="-std=c++2b -stdlib=libc++" -DMTP_BUILD_TEST=ON -DMTP_BUILD_MODULE=ON -DMTP_USE_STD_MODULE=ON
cmake --build build -j$(nproc)
ctest --test-dir build/test -j$(nproc)
```


# Links
1. [p0843: inplace_vector](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0843r14.html)

2. [p1144: std::is_trivially_relocatable](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p1144r12.html)

3. [cppcon 2021: david stone - implementing static_vector](https://www.youtube.com/watch?v=I8QJLGI0GOE)

4. [arthur o'dwyer: who uses p2786 & p1144 for trivial relocation?](https://quuxplusone.github.io/blog/2024/06/15/who-uses-trivial-relocation/)
