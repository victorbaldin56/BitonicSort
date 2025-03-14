/**
 * Bitonic sort OpenCL implementation.
 */

#include "cl_sort/cl_bitonic_sort.hh"

#include <cassert>
#include <numeric>

namespace {

template <typename Integer,
          typename = std::enable_if<std::is_integral_v<Integer>>>
constexpr Integer nextPowerOfTwo(Integer n) {
  --n;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  if constexpr (sizeof(n) > 1) { n |= n >> 8;  }
  if constexpr (sizeof(n) > 2) { n |= n >> 16; }
  if constexpr (sizeof(n) > 4) { n |= n >> 32; }
  ++n;
  return n;
}

}  // namespace

namespace bts {

void BitonicSorter::sort(std::vector<int>& data) {
  auto old_data_sz = data.size();
  prepareData(data);
  auto new_data_sz = data.size();
  assert((new_data_sz & (new_data_sz - 1)) == 0);

  auto global_size = new_data_sz;
  auto local_size = std::min(global_size, kMaxLocalSize);

  auto buffer = cl::Buffer(ctx_, CL_MEM_READ_WRITE, sizeof(int) * new_data_sz);
  cl::copy(queue_, data.begin(), data.end(), buffer);

  auto local =
      static_cast<cl::LocalSpaceArg>(cl::Local(local_size * sizeof(int)));

  auto bitonic_split = cl::Kernel(program_, "bitonicSplit");
  auto bitonic_merge = cl::Kernel(program_, "bitonicMerge");

  bitonic_split.setArg(0, buffer);
  bitonic_split.setArg(1, local);

  auto events = std::vector<cl::Event>();
  runKernel(bitonic_split, global_size, local_size, events);
  events.front().wait();

  events.clear();

  for (auto stage = std::size_t{2}; stage <= global_size; stage <<= 1) {
    for (auto step = stage >> 1; step > 0; step >>= 1) {
      bitonic_merge.setArg(0, buffer);
      bitonic_merge.setArg(1, stage);
      bitonic_merge.setArg(2, step);

      runKernel(bitonic_merge, global_size, local_size, events);
    }
  }

  std::for_each(events.begin(), events.end(), [](auto& evt) { evt.wait(); });

  cl::copy(queue_, buffer, data.begin(), data.begin() + old_data_sz);
  data.resize(old_data_sz);
}

void BitonicSorter::runKernel(const cl::Kernel& kernel,
                              std::size_t global_size,
                              std::size_t local_size,
                              std::vector<cl::Event>& events) {
  auto evt = cl::Event();
  queue_.enqueueNDRangeKernel(
      kernel, cl::NullRange, global_size, local_size, nullptr, &evt);
  events.push_back(evt);
}

cl::Platform BitonicSorter::selectPlatform() {
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

bool BitonicSorter::selectPlatformByType(
    int device_type,
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

cl::Device BitonicSorter::selectDevice(const cl::Platform& pl) {
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

cl::Context BitonicSorter::getDeviceContext(const cl::Device& dev) {
  return cl::Context(dev);
}

std::string BitonicSorter::readKernelFromFile(
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

/**
 * Resize vector to closest power of two
 */
void BitonicSorter::prepareData(std::vector<int>& data) {
  auto old_sz = data.size();
  auto new_sz = nextPowerOfTwo(old_sz);
  data.reserve(new_sz);

  auto elem = std::numeric_limits<int>::max();  // to not affect final result
  while (data.size() < new_sz) {
    data.push_back(elem);
  }
}

} // namespace ocl
