#include "cl_app/cl_app.hh"

#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

namespace {

auto findPlatform(const std::string& name) {
  std::vector<cl::Platform> pls;
  cl::Platform::get(&pls);
  if (pls.empty()) {
    throw std::runtime_error("No OpenCL platforms were found");
  }

  auto pls_begin = pls.cbegin();
  auto pls_end = pls.cend();
  auto it = std::find_if(pls_begin, pls_end, [&](const cl::Platform& p) {
    return p.getInfo<CL_PLATFORM_NAME>() == name;
  });
  if (it == pls_end) {
    throw std::runtime_error("OpenCL platform '" + name + "' not found");
  }

  return *it;
}

auto findDevice(const std::vector<cl::Device>& devs, const std::string& name) {
  auto devs_begin = devs.cbegin();
  auto devs_end = devs.cend();
  auto it = std::find_if(devs_begin, devs_end, [&](const cl::Device& dev) {
    return dev.getInfo<CL_DEVICE_NAME>() == name;
  });
  if (it == devs_end) {
    throw std::runtime_error("OpenCL device '" + name + "' not found");
  }

  return *it;
}

}  // namespace

namespace cl_app {

cl::Platform ClApplication::selectPlatform(const Config& config) {
  return config.platform_name.has_value()
             ? findPlatform(config.platform_name.value())
             : cl::Platform::getDefault();
}

cl::Device ClApplication::selectDevice(const cl::Platform& pl,
                                       const Config& cfg) {
  std::vector<cl::Device> devs;
  pl.getDevices(CL_DEVICE_TYPE_ALL, &devs);
  if (devs.empty()) {
    throw std::runtime_error("No OpenCL devices in platform were found");
  }

  return cfg.device_name.has_value() ? findDevice(devs, cfg.device_name.value())
                                     : devs.front();
}

std::string ClApplication::loadShader(const std::filesystem::path& path) {
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

}  // namespace cl_app
