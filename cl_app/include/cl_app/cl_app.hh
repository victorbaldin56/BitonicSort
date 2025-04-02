#pragma once

#include <filesystem>
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

template <typename It>
inline constexpr bool IsInputIterator =
    std::is_base_of_v<std::input_iterator_tag,
                      typename std::iterator_traits<It>::iterator_category>;

template <typename It>
inline constexpr bool IsOutputIterator =
    std::is_base_of_v<std::input_iterator_tag,
                      typename std::iterator_traits<It>::iterator_category>;

template <typename It>
inline constexpr bool IsIterator = IsInputIterator<It> || IsOutputIterator<It>;

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

  auto allocateDeviceBuffer(cl_mem_flags mode, std::size_t sz) const {
    return cl::Buffer(ctx_, mode, sz);
  }

  template <typename It, typename = std::enable_if_t<IsInputIterator<It>>>
  auto copy(It first, It last, cl::Buffer& buf) const {
    return cl::copy(queue_, first, last, buf);
  }
  template <typename It, typename = std::enable_if_t<IsIterator<It>>>
  auto copy(cl::Buffer& buf, It first, It last) const {
    return cl::copy(queue_, buf, first, last);
  }

  auto getKernel(const char* kname) const {
    return cl::Kernel(program_, kname);
  }

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
