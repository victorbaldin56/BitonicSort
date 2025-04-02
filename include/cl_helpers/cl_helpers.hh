#pragma once

#include <filesystem>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

namespace cl_helpers {

cl::Platform selectPlatform();
cl::Device selectDevice(const cl::Platform& pl);
cl::Context getDeviceContext(const cl::Device& dev);
std::string readKernelFromFile(const std::filesystem::path& path);
bool selectPlatformByType(int device_type, const std::vector<cl::Platform>& pls,
                          cl::Platform& p);
void runKernel(const cl::CommandQueue& queue, const cl::Kernel& kernel,
               std::size_t global_size, std::size_t local_size,
               std::vector<cl::Event>& events);

}  // namespace cl_helpers
