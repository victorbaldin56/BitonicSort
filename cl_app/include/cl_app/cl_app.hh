#pragma once

#include <filesystem>
#include <iostream>
#include <iterator>
#include <type_traits>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

namespace cl_app {

void dumpAllDevices(const std::vector<cl::Platform>& pls, std::ostream& os);

struct Config final {
  std::optional<std::string> platform_name;
  std::optional<std::string> device_name;
};

class ClApplication final {
 public:
  ClApplication(const std::filesystem::path& shader_path,
                const Config& cfg = {})
      : pls_(initPlatforms()),
        pl_(selectPlatform(pls_, cfg)),
        dev_(selectDevice(pl_, cfg)),
        ctx_(getDeviceContext(dev_)),
        queue_(ctx_, dev_, 0),
        cfg_(cfg),
        shader_(loadShader(shader_path)),
        program_(ctx_, shader_, true) {}

 public:
  const std::vector<cl::Platform>& getPlatforms() const { return pls_; }

 public:
  auto allocateBuffer(cl_mem_flags mode, std::size_t sz) const {
    return cl::Buffer(ctx_, mode, sz);
  }

  template <typename It>
  auto copy(It first, It last, cl::Buffer& buf) const {
    return cl::copy(queue_, first, last, buf);
  }
  template <typename It>
  auto copy(cl::Buffer& buf, It first, It last) const {
    return cl::copy(queue_, buf, first, last);
  }

  auto getKernel(const std::string& kname) const {
    return cl::Kernel(program_, kname.c_str());
  }

  auto runKernel(const cl::Kernel& kernel, std::size_t global_size,
                 std::size_t local_size, std::vector<cl::Event>& events) const {
    auto evt = cl::Event();
    queue_.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size,
                                nullptr, &evt);
    events.push_back(evt);
  }

 private:
  static std::vector<cl::Platform> initPlatforms();
  static cl::Platform selectPlatform(const std::vector<cl::Platform>& pls,
                                     const Config& config);
  static cl::Device selectDevice(const cl::Platform& pl, const Config& cfg);
  static std::string loadShader(const std::filesystem::path& path);
  static bool selectPlatformByType(int device_type,
                                   const std::vector<cl::Platform>& pls,
                                   cl::Platform& p);
  static cl::Context getDeviceContext(const cl::Device& dev) {
    return cl::Context(dev);
  }

 private:
  std::vector<cl::Platform> pls_;
  cl::Platform pl_;
  cl::Device dev_;
  cl::Context ctx_;
  cl::CommandQueue queue_;
  Config cfg_;
  std::string shader_;
  cl::Program program_;
};

}  // namespace cl_app
