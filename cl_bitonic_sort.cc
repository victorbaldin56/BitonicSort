/**
 * Bitonic sort OpenCL implementation.
 */
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

  using bitonicSortT = cl::KernelFunctor<cl::Buffer, int>;

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

namespace {

template <typename It>
void randomFill(It start, It end, TYPE low, TYPE high) {
  static std::mt19937_64 mt;
  std::uniform_int_distribution<int> dist(low, high);
  std::for_each(start, end, [&dist](TYPE& v){ v = dist(mt); });
}

} // helpers

#ifdef TESTING
TEST(bitonic, test) {

}
#endif

int main() {
  int n;
#ifdef BENCHMARK
  n = kDataSize;
#else
  n = *std::istream_iterator<int>(std::cin);
#endif

  std::vector<TYPE> data(n);

#ifdef BENCHMARK
  randomFill(data.begin(), data.end(), -1000000, 1000000);
#else
  std::copy_n(std::istream_iterator<TYPE>(std::cin), n, data.begin());
#endif

#ifdef BENCHMARK
  std::vector copy(data);
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();
  std::sort(copy.begin(), copy.end());
  end = std::chrono::high_resolution_clock::now();
  long dur
      = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  std::cout << "CPU time measured: " << dur << " ms\n";
#endif

  // sorting with OpenCL
  ocl::App app;
  auto evt = app.bitonicSort(data.data(), data.size());

#ifdef BENCHMARK
  cl_ulong gpu_start, gpu_end;
  gpu_start = evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  gpu_end = evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  long gpu_dur = (gpu_end - gpu_start) / 1000000;
  std::cout << "GPU time measured: " << gpu_dur << " ms\n";

  assert(data == copy && "Failed GPU sort test");
#endif

#ifndef BENCHMARK
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
#endif

  return 0;
}

namespace ocl {

cl::Event App::bitonicSort(TYPE* data, std::size_t sz) {
  std::size_t buf_sz = sz * sizeof(TYPE);

  cl::Buffer buf(ctx_, CL_MEM_READ_WRITE, buf_sz);
  cl::copy(queue_, data, data + sz, buf);

  // loading kernel
  cl::Program program(ctx_, kernel_, true);
  bitonicSortT sort(program, "bitonicSort");

  cl::NDRange global_range(sz);
  cl::NDRange local_range(kLocalSize);
  cl::EnqueueArgs args(queue_, global_range, local_range);

  cl::Event evt = sort(args, buf, sz);
  evt.wait();

  cl::copy(queue_, buf, data, data + sz);
  return evt; // for profiling
}

cl::Platform App::selectPlatform() {
  cl::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  for (auto p : platforms) {
    // note: usage of p() for plain id
    cl_uint numdevices = 0;
    ::clGetDeviceIDs(p(), CL_DEVICE_TYPE_GPU, 0, NULL, &numdevices);
    if (numdevices > 0)
      return cl::Platform(p); // retain?
  }
  throw std::runtime_error("No platform selected");
}

cl::Context App::getGpuContext(cl_platform_id pid) {
  cl_context_properties properties[] = {
      CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(pid),
      0
  };

  return cl::Context(CL_DEVICE_TYPE_GPU, properties);
}

std::string App::readFile(const char *path) {
  std::string code;
  std::ifstream shader_file;
  shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  shader_file.open(path);
  std::stringstream shader_stream;
  shader_stream << shader_file.rdbuf();
  shader_file.close();
  code = shader_stream.str();
  return code;
}

} // namespace ocl
