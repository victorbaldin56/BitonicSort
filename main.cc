#include "cl_sort/cl_bitonic_sort.hh"

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
#if defined(BENCHMARK ) || defined(SELFCHECK)
  n = kDataSize;
#else
  n = *std::istream_iterator<int>(std::cin);
#endif

  std::vector<TYPE> data(n);

  // for benchmark or functionality test we choose random data
#if defined(BENCHMARK) || defined(SELFCHECK)
  randomFill(data.begin(), data.end(), -1000000, 1000000);
#else
  std::copy_n(std::istream_iterator<TYPE>(std::cin), n, data.begin());
#endif

#ifdef BENCHMARK
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();
#endif

#if defined(BENCHMARK) || defined(SELFCHECK)
  std::vector copy(data);
  std::sort(copy.begin(), copy.end());
#endif

#ifdef BENCHMARK
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
#endif

#ifdef SELFCHECK
  if (data != copy) {
    throw std::runtime_error("SELFCHECK FAILED");
  }
#endif

#if !defined(BENCHMARK) && !defined(SELFCHECK)
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
#endif

  return 0;
}
