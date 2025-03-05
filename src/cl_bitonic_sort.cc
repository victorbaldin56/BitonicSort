/**
 * Bitonic sort OpenCL implementation.
 */

#include "cl_sort/cl_bitonic_sort.hh"

#include <cassert>
#include <numeric>

namespace ocl {

void BitonicSorter::sort(std::vector<int>& data) {
  auto old_data_sz = data.size();
  prepareData(data);
  auto new_data_sz = data.size();

  auto global_size = new_data_sz;
  auto local_size = std::min(global_size, kMaxLocalSize);

  auto buffer = cl::Buffer(ctx_, CL_MEM_READ_WRITE, sizeof(int) * new_data_sz);
  queue_.enqueueWriteBuffer(
      buffer, true, 0, sizeof(int) * new_data_sz, data.data());

  auto local =
      static_cast<cl::LocalSpaceArg>(cl::Local(local_size * sizeof(int)));

  // compiles shader immediately
  auto program = cl::Program(ctx_, shader_, true);

  auto bitonic_split = cl::Kernel(program, "bitonicSplit");
  auto bitonic_merge = cl::Kernel(program, "bitonicMerge");

  bitonic_split.setArg(0, buffer);
  bitonic_split.setArg(1, local);

  auto events = std::vector<cl::Event>();
  runKernel(bitonic_split, global_size, local_size, events);
  events.front().wait();

  events.clear();

  for (auto stage = local_size << 1; stage <= global_size; stage <<= 1) {
    for (auto step = stage >> 1; step > 0; step >>= 1) {
      bitonic_merge.setArg(0, buffer);
      bitonic_merge.setArg(1, stage);
      bitonic_merge.setArg(2, step);

      runKernel(bitonic_merge, global_size, local_size, events);
    }
  }

  std::for_each(events.begin(), events.end(), [](auto& evt) { evt.wait(); });

  cl::copy(queue_, buffer, data.begin(), data.end());
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
  return platforms.front();
}

cl::Device BitonicSorter::selectDevice(cl::Platform pl) {
  auto devices = std::vector<cl::Device>();
  pl.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  if (devices.empty()) {
    throw std::runtime_error("No OpenCL devices were found");
  }
  return devices.front();
}

cl::Context BitonicSorter::getGpuContext(cl::Device dev) {
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
  auto elem = std::numeric_limits<int>::max();  // to not affect final result

  auto old_sz = data.size();
  auto new_size_log = static_cast<std::size_t>(std::ceil(std::log2(old_sz)));
  auto new_sz = 1 << new_size_log;
  data.reserve(new_sz);

  while (data.size() < new_sz) {
    data.push_back(elem);
  }
}

} // namespace ocl
