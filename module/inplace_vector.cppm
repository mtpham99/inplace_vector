module;

// -------------------------------------------------------------------------------------------------

#if !defined(MTP_USE_STD_MODULE) && defined(__cpp_lib_modules)
#  define MTP_USE_STD_MODULE
#endif

// -------------------------------------------------------------------------------------------------

#ifndef MTP_USE_STD_MODULE
#  include <version>

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

// -------------------------------------------------------------------------------------------------

export module mtp.inplace_vector;

#ifdef MTP_USE_STD_MODULE
import std;
#endif

#ifdef __clang__
#  pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#endif

#define MTP_BUILD_MODULE
#define MTP_EXPORT export
#include <mtp/inplace_vector.hpp>
