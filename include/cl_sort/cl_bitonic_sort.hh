#pragma once

#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

namespace ocl {

constexpr std::size_t kLocalSize = 16;

struct Config {
  const char* path_ = "bitonic_sort.cl";
  cl::QueueProperties queue_props_ =
      cl::QueueProperties::Profiling | cl::QueueProperties::OutOfOrder;
};

class App final {
  cl::Platform pl_;
  cl::Device dev_;
  cl::Context ctx_;
  cl::CommandQueue queue_;
  Config cfg_;
  std::string kernel_;

  static cl::Platform selectPlatform();
  static cl::Device selectDevice(cl::Platform pl);
  static cl::Context getGpuContext(cl::Device dev);
  static std::string readKernelFromFile(const char* path);

  using bitonicSortT
      = cl::KernelFunctor<cl::Buffer, cl::LocalSpaceArg, cl_ulong>;

 public:
  App(Config cfg = {})
      : pl_(selectPlatform()),
        dev_(selectDevice(pl_)),
        ctx_(getGpuContext(dev_)),
        queue_(ctx_, dev_, 0),
        cfg_(cfg),
        kernel_(readKernelFromFile(cfg.path_)) {}

  cl::Event bitonicSort(int* data, std::size_t sz);
};

} // namespace ocl
