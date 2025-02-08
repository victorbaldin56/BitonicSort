cmake_minimum_required(VERSION 3.14)
project(BitonicSort)

# find_package(OpenCLHeaders REQUIRED)
# find_package(OpenCLICDLoader REQUIRED)
# find_package(OpenCLHeadersCpp REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(bitonic
  main.cc
)

target_include_directories(bitonic PRIVATE ${OpenCL_INCLUDE_DIR})
target_link_libraries(bitonic OpenCL::OpenCL)
target_compile_features(bitonic PRIVATE cxx_std_17)
