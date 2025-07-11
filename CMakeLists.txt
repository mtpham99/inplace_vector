cmake_minimum_required(VERSION 3.22)

if(CMAKE_VERSION GREATER_EQUAL 3.30)
  # https://www.kitware.com/import-std-in-cmake-3-30
  # https://github.com/Kitware/CMake/blob/v3.30.0/Help/dev/experimental.rst
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
endif()

project(mtp_inplace_vector LANGUAGES CXX)

option(MTP_BUILD_TEST "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(MTP_NO_EXCEPTIONS "Disable exceptions" OFF)
option(MTP_BUILD_MODULE "Build as module" OFF)
option(MTP_USE_STD_MODULE "Use c++23 std module" OFF)

if(CMAKE_VERSION LESS 3.28 AND MTP_BUILD_MODULE)
  message(FATAL_ERROR "CMake version >= 3.28 required for building modules.")
endif()
if(CMAKE_VERSION LESS 3.30 AND MTP_USE_STD_MODULE)
  message(
    FATAL_ERROR "CMake version >= 3.30 required for using c++23 std module.")
endif()
if(MTP_USE_STD_MODULE AND NOT MTP_BUILD_MODULE)
  message(FATAL_ERROR "Must use module build if using c++23 std module.")
endif()

if(MTP_BUILD_MODULE)
  set(MTP_TARGET_LIB_SCOPE PRIVATE)
  add_library(mtp_inplace_vector)
  add_library(mtp::inplace_vector ALIAS mtp_inplace_vector)
  target_sources(
    mtp_inplace_vector
    PRIVATE FILE_SET HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/include FILES
            ${PROJECT_SOURCE_DIR}/include/mtp/inplace_vector.hpp
    PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}/module FILES
           ${PROJECT_SOURCE_DIR}/module/inplace_vector.cppm)

  target_compile_features(
    mtp_inplace_vector
    PRIVATE cxx_std_20
    INTERFACE cxx_std_20)

  if(MTP_USE_STD_MODULE)
    target_compile_definitions(mtp_inplace_vector PRIVATE MTP_USE_STD_MODULE)
    target_compile_features(
      mtp_inplace_vector
      PRIVATE cxx_std_23
      INTERFACE cxx_std_20)
    set_target_properties(mtp_inplace_vector PROPERTIES CXX_MODULE_STD ON)
  endif()

else() # NOT MTP_BUILD_MODULE (header only build)
  set(MTP_TARGET_LIB_SCOPE INTERFACE)
  add_library(mtp_inplace_vector INTERFACE)
  add_library(mtp::inplace_vector ALIAS mtp_inplace_vector)
  target_sources(
    mtp_inplace_vector
    INTERFACE FILE_SET HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/include FILES
              ${PROJECT_SOURCE_DIR}/include/mtp/inplace_vector.hpp)

  target_compile_features(mtp_inplace_vector INTERFACE cxx_std_20)
endif()

if(MTP_NO_EXCEPTIONS)
  target_compile_definitions(mtp_inplace_vector ${MTP_TARGET_LIB_SCOPE}
                                                MTP_NO_EXCEPTIONS)
endif()

if(MTP_BUILD_TEST)
  add_subdirectory(test)
endif()
