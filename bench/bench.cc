#include <algorithm>

#include "benchmark/benchmark.h"

#include "cl_sort/cl_bitonic_sort.hh"

constexpr auto kNumIterations = 10;

class FileSortFixture : public benchmark::Fixture {
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

BENCHMARK_DEFINE_F(FileSortFixture, BitonicSort)(benchmark::State& state) {
  auto data = original_data_;
  for (auto _ : state) {
    sorter_.sort(data);
  }
}
BENCHMARK_REGISTER_F(FileSortFixture, BitonicSort)
  ->DenseRange(1, FileSortFixture::kBenchNum)
  ->Iterations(kNumIterations);

BENCHMARK_DEFINE_F(FileSortFixture, StdSort)(benchmark::State& state) {
  auto data = original_data_;
  for (auto _ : state) {
    std::sort(data.begin(), data.end());
  }
}
BENCHMARK_REGISTER_F(FileSortFixture, StdSort)
  ->DenseRange(1, FileSortFixture::kBenchNum)
  ->Iterations(kNumIterations);

BENCHMARK_MAIN();
