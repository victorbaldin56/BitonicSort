#include "cl_sort/cl_bitonic_sort.hh"

namespace {

constexpr std::size_t kDataSize = 0x10;

template <typename It>
void randomFill(It start, It end, int low, int high) {
  static std::mt19937_64 mt;
  std::uniform_int_distribution<int> dist(low, high);
  std::for_each(start, end, [&dist](int& v){ v = dist(mt); });
}

} // helpers

int main() {
  std::size_t n;
#if defined(BENCHMARK) || defined(SELFCHECK)
  n = kDataSize;
#else
  n = *std::istream_iterator<int>(std::cin);
#endif

  std::vector<int> data_for_gpu(n);

  // for benchmark or functionality test we choose random data
#if defined(BENCHMARK) || defined(SELFCHECK)
  randomFill(data_for_gpu.begin(), data_for_gpu.end(), -100, 100);
#else
  std::copy_n(std::istream_iterator<int>(std::cin), n, data.begin());

#endif

#ifdef BENCHMARK
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();
#endif

#if defined(BENCHMARK) || defined(SELFCHECK)
  std::vector data_for_cpu(data_for_gpu);
  std::sort(data_for_cpu.begin(), data_for_cpu.end());
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
  auto evt = app.bitonicSort(data_for_gpu.data(), data_for_gpu.size());

#ifdef BENCHMARK
  cl_ulong gpu_start, gpu_end;
  gpu_start = evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  gpu_end = evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  long gpu_dur = (gpu_end - gpu_start) / 1000000;
  std::cout << "GPU time measured: " << gpu_dur << " ms\n";
#endif

#ifdef SELFCHECK
  if (data_for_gpu != data_for_cpu) {
    std::cerr << "Selfcheck failed\n"
              << "Expected: ";
    std::copy(data_for_cpu.begin(), data_for_cpu.end(),
              std::ostream_iterator<int>(std::cerr, " "));
    std::cerr << "\n"
              << "Got: ";
    std::copy(data_for_gpu.begin(), data_for_gpu.end(),
              std::ostream_iterator<int>(std::cerr, " "));
    std::cerr << "\n";
    return EXIT_FAILURE;
  }
#endif

#if !defined(BENCHMARK) && !defined(SELFCHECK)
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
#endif

  return 0;
}
