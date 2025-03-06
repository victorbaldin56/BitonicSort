#include <algorithm>
#include <chrono>

#include "benchmark/benchmark.h"

#include "cl_sort/cl_bitonic_sort.hh"

constexpr auto kNumIterations = 100;

class SortCompareFixture : public benchmark::Fixture {
public:
  void SetUp(const benchmark::State& state) override {
    input_dir_ =
        std::filesystem::absolute(PROJECT_ROOT).append(kRelativeInputPath);

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
  bts::BitonicSorter sorter_;
  int test_num_;

  static constexpr auto kBenchNum = 10;
  static constexpr auto kRelativeInputPath = "tests/e2e/generated/input";
};

BENCHMARK_DEFINE_F(SortCompareFixture, CompareSorts)(benchmark::State& state) {
  auto bitonic_sorter = bts::BitonicSorter();

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

    state.counters["bitonic_time (ms)"] =
        benchmark::Counter(
            std::chrono::duration_cast<std::chrono::duration<double>>(
                bitonic_end - bitonic_start).count() * 1000,
            benchmark::Counter::kAvgThreads,
            benchmark::Counter::OneK::kIs1024);

    state.counters["std_time (ms)"] =
        benchmark::Counter(
            std::chrono::duration_cast<std::chrono::duration<double>>(
                std_end - std_start).count() * 1000,
            benchmark::Counter::kAvgThreads,
            benchmark::Counter::OneK::kIs1024);
  }
}

BENCHMARK_REGISTER_F(SortCompareFixture, CompareSorts)
  ->DenseRange(1, SortCompareFixture::kBenchNum)
  ->Iterations(kNumIterations);

BENCHMARK_MAIN();
