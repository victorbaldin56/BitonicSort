/**
 * Bitonic sort OpenCL implementation.
 */

#include "cl_sort/cl_bitonic_sort.hh"

#include <cassert>

namespace ocl {

cl::Event App::bitonicSort(TYPE* data, std::size_t sz) {
  std::size_t buf_sz = sz * sizeof(TYPE);

  cl::Buffer buf(ctx_, CL_MEM_READ_WRITE, buf_sz);
  cl::copy(queue_, data, data + sz, buf);

  // loading kernel
  cl::Program program(ctx_, kernel_, true);
  program.build("-DTYPE=float");
  bitonicSortT sort(program, "bitonicSort");

  cl::NDRange global_range(sz);
  cl::NDRange local_range(kLocalSize);
  cl::EnqueueArgs args(queue_, global_range, local_range);

  cl::Event evt = sort(args, buf, cl::Local(sizeof(float) * kLocalSize), sz);
  evt.wait();

  cl::copy(queue_, buf, data, data + sz);
  return evt; // for profiling
}

cl::Platform App::selectPlatform() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  assert(!platforms.empty());
  return platforms.front();
}

cl::Context App::getGpuContext(cl::Platform pl) {
  std::vector<cl::Device> devices;
  pl.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  assert(!devices.empty());
  return cl::Context(devices.front());
}

std::string App::readKernelFromFile(const char *path) {
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
