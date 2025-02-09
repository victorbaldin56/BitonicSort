/**
 * Bitonic sort OpenCL implementation.
 */

#include "cl_sort/cl_bitonic_sort.hh"

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
