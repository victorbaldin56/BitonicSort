#include <algorithm>
#include <chrono>

#include "benchmark/benchmark.h"

#include "cl_sort/cl_bitonic_sort.hh"

constexpr auto kNumIterations = 1;

class SortCompareFixture : public benchmark::Fixture {
public:
  void SetUp(const benchmark::State& state) override {
    input_dir_ = std::filesystem::absolute(__FILE__).parent_path().append(
        kRelativeInputPath);

    test_num_ = state.range(0);
    loadTestData(test_num_);
  }

  void loadTestData(int test_id) {
    auto filepath =
        input_dir_.append(std::string("test_") +
                          (test_id < 10 ? "0" : "") +
                          std::to_string(test_id) + ".in");
    auto file = std::ifstream(filepath);

    if (!file.is_open()) {
      std::cerr << "Failed to open: " << filepath << "\n";
      return;
    }

    auto line = std::string();
    std::getline(file, line);
    auto iss = std::istringstream(line);

    auto size = std::size_t{};
    iss >> size;
    original_data_.reserve(size);

    auto value = int{};
    while (iss >> value) {
      original_data_.push_back(value);
    }
  }

  std::filesystem::path input_dir_;
  std::vector<int> original_data_;
  cl_sort::BitonicSorter sorter_;
  int test_num_;

  static constexpr auto kBenchNum = 10;
  static constexpr auto kRelativeInputPath = "data";
};

BENCHMARK_DEFINE_F(SortCompareFixture, CompareSorts)(benchmark::State& state) {
  auto bitonic_sorter = cl_sort::BitonicSorter();

  for (auto _ : state) {
    state.PauseTiming();
    auto bitonic_data = original_data_;
    auto std_data = original_data_;

    auto bitonic_start = std::chrono::high_resolution_clock::now();
    bitonic_sorter.sort(bitonic_data);
    auto bitonic_end = std::chrono::high_resolution_clock::now();

    auto std_start = std::chrono::high_resolution_clock::now();
    std::sort(std_data.begin(), std_data.end());
    auto std_end = std::chrono::high_resolution_clock::now();

    auto std_time =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            std_end - std_start).count() * 1000;

    auto bitonic_time =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            bitonic_end - bitonic_start).count() * 1000;

    state.counters["size"] = benchmark::Counter(original_data_.size());
    state.counters["bitonic_time (ms)"] = benchmark::Counter(bitonic_time);
    state.counters["std_time (ms)"] = benchmark::Counter(std_time);
    state.counters["std_time / bitonic_time"] =
        benchmark::Counter(std_time / bitonic_time);
  }
}

BENCHMARK_REGISTER_F(SortCompareFixture, CompareSorts)
  ->DenseRange(1, SortCompareFixture::kBenchNum)
  ->Iterations(kNumIterations);

BENCHMARK_MAIN();
