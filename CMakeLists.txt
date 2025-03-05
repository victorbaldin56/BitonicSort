cmake_minimum_required(VERSION 3.14)
project(BitonicSort)

find_package(OpenCLHeadersCpp REQUIRED)

add_executable(bitonic cl_bitonic_sort.cc main.cc)

target_include_directories(bitonic PRIVATE ${OpenCL_INCLUDE_DIR})
target_include_directories(bitonic PRIVATE include)
target_compile_features(bitonic PRIVATE cxx_std_17)
target_link_libraries(bitonic OpenCL::HeadersCpp)
