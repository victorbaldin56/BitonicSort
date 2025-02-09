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

// no another way
#ifndef TYPE
#define TYPE int
#endif

namespace ocl {

constexpr std::size_t kLocalSize = 1;
constexpr std::size_t kDataSize = 1000000;

struct Config {
  const char* path_ = "bitonic_sort.cl";
  cl::QueueProperties queue_props_ =
      cl::QueueProperties::Profiling | cl::QueueProperties::OutOfOrder;
};

class App final {
  cl::Platform pl_;
  cl::Context ctx_;
  cl::CommandQueue queue_;
  Config cfg_;
  std::string kernel_;

  static cl::Platform selectPlatform();
  static cl::Context getGpuContext(cl_platform_id pid);
  static std::string readFile(const char* path);

  using bitonicSortT = cl::KernelFunctor<cl::Buffer, cl_ulong>;

 public:
  App(Config cfg = {})
      : pl_(selectPlatform()),
        ctx_(getGpuContext(pl_())),
        queue_(ctx_, cfg.queue_props_),
        cfg_(cfg),
        kernel_(readFile(cfg.path_)) {}

  cl::Event bitonicSort(TYPE* data, std::size_t sz);
};

} // namespace ocl
