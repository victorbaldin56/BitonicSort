cmake_minimum_required(VERSION 3.14)
project(BitonicSort)

find_package(OpenCL REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)

add_executable(bitonic src/cl_bitonic_sort.cc src/main.cc)

target_include_directories(bitonic PRIVATE include)
target_compile_features(bitonic PRIVATE cxx_std_17)
target_link_libraries(bitonic OpenCL::HeadersCpp OpenCL::OpenCL)

if (${BUILD_TESTING})
  enable_testing()
  add_subdirectory(tests)
endif()
