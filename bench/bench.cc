#include <algorithm>

#include "benchmark/benchmark.h"

#include "cl_sort/cl_bitonic_sort.hh"

const std::string INPUT_DIR = "inputs/"; // Update this path

class FileSortFixture : public benchmark::Fixture {
public:
  void SetUp(const benchmark::State& state) override {
    test_num_ = state.range(0);
    LoadTestData(test_num_);
  }

  void LoadTestData(int test_id) {
    std::string filename = INPUT_DIR + "test_" +
                          (test_id < 10 ? "0" : "") +
                          std::to_string(test_id) + ".in";
    std::ifstream file(filename);

    if (!file.is_open()) {
      std::cerr << "Failed to open: " << filename << "\n";
      return;
    }

    // Read entire line and parse
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);

    // First number is size (optional), rest are elements
    int value;
    while (iss >> value) {
      original_data_.push_back(value);
    }
  }

  std::vector<int> original_data_;
  int test_num_;

  static constexpr auto kBenchNum = 10;
};

// Bitonic Sort Benchmark
BENCHMARK_DEFINE_F(FileSortFixture, BitonicSort)(benchmark::State& state) {
  bts::BitonicSorter sorter;
  for (auto _ : state) {
    auto data = original_data_; // Copy includes padding in sorter.sort()
    sorter.sort(data);
  }
}
BENCHMARK_REGISTER_F(FileSortFixture, BitonicSort)
  ->DenseRange(1, 10); // Test numbers 1-10

// std::sort Benchmark
BENCHMARK_DEFINE_F(FileSortFixture, StdSort)(benchmark::State& state) {
  for (auto _ : state) {
    auto data = original_data_; // Copy original (unpadded) data
    std::sort(data.begin(), data.end());
  }
}
BENCHMARK_REGISTER_F(FileSortFixture, StdSort)
  ->DenseRange(1, 10);

BENCHMARK_MAIN();