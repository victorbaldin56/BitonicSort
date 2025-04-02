#include "cl_app/cl_app.hh"

#include <fstream>
#include <sstream>

namespace cl_app {

cl::Platform ClApplication::selectPlatform() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (platforms.empty()) {
    throw std::runtime_error("No OpenCL platforms were found");
  }

  auto res = cl::Platform();
  if (!selectPlatformByType(CL_DEVICE_TYPE_GPU, platforms, res)) {
    if (!selectPlatformByType(CL_DEVICE_TYPE_CPU, platforms, res)) {
      throw std::runtime_error("No OpenCL platforms with devices were found");
    }
  }
  return res;
}

bool ClApplication::selectPlatformByType(int device_type,
                                         const std::vector<cl::Platform>& pls,
                                         cl::Platform& pl) {
  auto pl_begin = pls.cbegin();
  auto pl_end = pls.cend();
  auto pl_it = std::find_if(pl_begin, pl_end, [&](auto&& p) {
    auto devs = std::vector<cl::Device>();
    p.getDevices(device_type, &devs);
    return !devs.empty();
  });

  if (pl_it == pl_end) {
    return false;
  }
  pl = *pl_it;
  return true;
}

cl::Device ClApplication::selectDevice(const cl::Platform& pl) {
  auto devices = std::vector<cl::Device>();
  pl.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  if (devices.empty()) {
    pl.getDevices(CL_DEVICE_TYPE_CPU, &devices);
    if (devices.empty()) {
      throw std::runtime_error("No OpenCL devices were found");
    }
  }
  return devices.front();
}

cl::Context ClApplication::getDeviceContext(const cl::Device& dev) {
  return cl::Context(dev);
}

std::string ClApplication::readKernelFromFile(
    const std::filesystem::path& path) {
  auto code = std::string();
  auto shader_file = std::ifstream();
  shader_file.open(path);
  if (!shader_file.is_open()) {
    std::string what = std::string("Failed to open file ") + path.string();
    throw std::runtime_error(what);
  }
  auto shader_stream = std::stringstream();
  shader_stream << shader_file.rdbuf();
  shader_file.close();
  code = shader_stream.str();
  return code;
}

void ClApplication::runKernel(const cl::Kernel& kernel, std::size_t global_size,
                              std::size_t local_size,
                              std::vector<cl::Event>& events) const {
  auto evt = cl::Event();
  queue_.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size,
                              nullptr, &evt);
  events.push_back(evt);
}

}  // namespace cl_app
