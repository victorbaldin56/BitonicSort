#pragma once

#include <filesystem>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

namespace cl_app {

struct Config final {
  std::filesystem::path path;
};

class ClApplication final {
 public:
  ClApplication(Config cfg = {})
      : pl_(selectPlatform()),
        dev_(selectDevice(pl_)),
        ctx_(getDeviceContext(dev_)),
        queue_(ctx_, dev_, 0),
        cfg_(cfg),
        shader_(readKernelFromFile(cfg.path)),
        program_(ctx_, shader_, true) {}

  void runKernel(const cl::Kernel& kernel, std::size_t global_size,
                 std::size_t local_size, std::vector<cl::Event>& events) const;
  const cl::Context& context() const noexcept { return ctx_; }
  const cl::Program& program() const noexcept { return program_; }
  const cl::CommandQueue& queue() const noexcept { return queue_; }

 private:
  cl::Platform selectPlatform();
  cl::Device selectDevice(const cl::Platform& pl);
  cl::Context getDeviceContext(const cl::Device& dev);
  std::string readKernelFromFile(const std::filesystem::path& path);
  bool selectPlatformByType(int device_type,
                            const std::vector<cl::Platform>& pls,
                            cl::Platform& p);

 private:
  cl::Platform pl_;
  cl::Device dev_;
  cl::Context ctx_;
  cl::CommandQueue queue_;
  Config cfg_;
  std::string shader_;
  cl::Program program_;
};

}  // namespace cl_app
