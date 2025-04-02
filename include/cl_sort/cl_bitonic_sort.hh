#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "cl_helpers/cl_helpers.hh"

namespace bts {

struct Config {
  std::filesystem::path path_ =
      std::filesystem::absolute(PROJECT_ROOT).append("shaders/bitonic_sort.cl");
};

class BitonicSorter final {
 public:
  BitonicSorter(Config cfg = {})
      : pl_(cl_helpers::selectPlatform()),
        dev_(cl_helpers::selectDevice(pl_)),
        ctx_(cl_helpers::getDeviceContext(dev_)),
        queue_(ctx_, dev_, 0),
        cfg_(cfg),
        shader_(cl_helpers::readKernelFromFile(cfg.path_)),
        program_(ctx_, shader_, true) {}

  void sort(std::vector<int>& data) const;

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
