#pragma once

#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

namespace bts {

struct Config {
  std::filesystem::path path_ =
      std::filesystem::absolute(PROJECT_ROOT)
          .append("shaders/bitonic_sort.cl");
  cl::QueueProperties queue_props_ = cl::QueueProperties::OutOfOrder;
};

class BitonicSorter final {
 public:
  BitonicSorter(Config cfg = {})
      : pl_(selectPlatform()),
        dev_(selectDevice(pl_)),
        ctx_(getDeviceContext(dev_)),
        queue_(ctx_, dev_, 0),
        cfg_(cfg),
        shader_(readKernelFromFile(cfg.path_)),
        program_(ctx_, shader_, true) {}

  void sort(std::vector<int>& data) const;

 private:
  void runKernel(const cl::Kernel& kernel,
                 std::size_t global_size,
                 std::size_t local_size,
                 std::vector<cl::Event>& events) const;

 private:  // constructor helpers
  static cl::Platform selectPlatform();
  static cl::Device selectDevice(const cl::Platform& pl);
  static cl::Context getDeviceContext(const cl::Device& dev);
  static std::string readKernelFromFile(const std::filesystem::path& path);
  static bool selectPlatformByType(
      int device_type,
      const std::vector<cl::Platform>& pls,
      cl::Platform& p);

 private:  // helpers
  static void prepareData(std::vector<int>& data);

 private:
  cl::Platform pl_;
  cl::Device dev_;
  cl::Context ctx_;
  cl::CommandQueue queue_;
  Config cfg_;
  std::string shader_;
  cl::Program program_;

  static constexpr auto kMaxLocalSize = std::size_t{0x100};
};

} // namespace ocl
