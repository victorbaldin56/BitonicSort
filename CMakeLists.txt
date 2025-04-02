cmake_minimum_required(VERSION 3.14)
project(BitonicSort)

find_package(OpenCL REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)
find_package(benchmark REQUIRED)

add_library(bitonic src/cl_bitonic_sort.cc src/cl_helpers.cc)

target_include_directories(bitonic PUBLIC include)
target_compile_features(bitonic PUBLIC cxx_std_17)
target_link_libraries(bitonic OpenCL::HeadersCpp OpenCL::OpenCL)
target_compile_definitions(bitonic PUBLIC PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(bench)
add_subdirectory(driver)

option(ENABLE_TESTS "Enable testing" ON)
if (ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
