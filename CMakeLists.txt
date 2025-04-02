cmake_minimum_required(VERSION 3.14)
project(BitonicSort)

find_package(OpenCL REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)
find_package(benchmark REQUIRED)

add_subdirectory(cl_app)
add_subdirectory(cl_sort)
add_subdirectory(bench)
add_subdirectory(driver)

option(ENABLE_TESTS "Enable testing" ON)
if (ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
