#include "cl_app/cl_app.hh"

#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

namespace {

template <typename UnaryPred, typename ErrorHandler>
auto findPlatform(const std::vector<cl::Platform>& pls, UnaryPred pred,
                  ErrorHandler ehnd) {
  auto pl_begin = pls.cbegin();
  auto pl_end = pls.cend();
  auto it = std::find_if(pl_begin, pl_end, pred);
  if (it == pl_end) {
    ehnd();
  }
  return *it;
}

}  // namespace

namespace cl_app {

/**
 * Dumps all platforms with corresponding devices
 */
void dumpAllDevices(const std::vector<cl::Platform>& pls, std::ostream& os) {
  std::for_each(pls.cbegin(), pls.cend(), [&](const cl::Platform& p) {
    os << p.getInfo<CL_PLATFORM_NAME>() << ": ";

    std::vector<cl::Device> devs;
    p.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    std::transform(
        devs.cbegin(), devs.cend(), std::ostream_iterator<std::string>(os, " "),
        [](const cl::Device& d) { return d.getInfo<CL_DEVICE_NAME>(); });
    os << std::endl;
  });
}

std::vector<cl::Platform> ClApplication::initPlatforms() {
  std::vector<cl::Platform> pls;
  cl::Platform::get(&pls);
  if (pls.empty()) {
    throw std::runtime_error("No OpenCL platforms were found");
  }
  return pls;
}

cl::Platform ClApplication::selectPlatform(const std::vector<cl::Platform>& pls,
                                           const Config& config) {
  cl::Platform res;
  if (config.platform_name.has_value()) {
    auto ehnd = [&]() {
      auto what = "No OpenCL platforms named '" + config.platform_name.value() +
                  "' were found";
      throw std::runtime_error(what);
    };
    auto pred = [&](const cl::Platform& p) {
      return p.getInfo<CL_PLATFORM_NAME>() == config.platform_name.value();
    };
    res = findPlatform(pls, pred, ehnd);

    std::vector<cl::Device> devs;
    res.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    if (devs.empty()) {
      throw std::runtime_error(
          "No OpenCL devices were found on selected platform");
    }
  } else {
    auto ehnd = []() {
      throw std::runtime_error("No OpenCL devices were found");
    };
    auto pred = [](auto&& p) {
      std::vector<cl::Device> devs;
      p.getDevices(CL_DEVICE_TYPE_ALL, &devs);
      return !devs.empty();
    };
    res = findPlatform(pls, pred, ehnd);
  }

  return res;
}

cl::Device ClApplication::selectDevice(const cl::Platform& pl,
                                       const Config& cfg) {
  std::vector<cl::Device> devs;
  pl.getDevices(CL_DEVICE_TYPE_ALL, &devs);
  assert(!devs.empty());

  if (!cfg.device_name.has_value()) {
    return devs.front();
  }

  auto devs_begin = devs.cbegin();
  auto devs_end = devs.cend();
  auto it = std::find_if(devs_begin, devs_end, [&](const cl::Device& d) {
    return d.getInfo<CL_DEVICE_NAME>() == cfg.device_name.value();
  });
  if (it == devs_end) {
    throw std::runtime_error("No OpenCL devices named '" +
                             cfg.device_name.value() + "' were found");
  }

  return *it;
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
