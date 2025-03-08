# Bitonic sort

Bitonic mergesort is a well paralleled algorithm for sorting.
This is OpenCL bitonic sort implementation.

## Dependencies

* Git
* CMake
* C++ compiler + libstdc++ standard >= 17
* Python3
* `pip`
* OpenCL driver library

## Build

1. Clone:

   ```sh
   git clone https://github.com/victorbaldin56/BitonicSort.git
   cd BitonicSort
   ```

1. Create virtual environment for Python:

   ```sh
   python3 -m venv .venv
   source .venv/bin/activate
   ```

1. Install Conan:

   ```sh
   pip3 install conan
   conan profile detect --force
   ```

1. Install Conan requirements and toolchain:

   ```sh
   conan install . --output-folder=build --build=missing
   ```

1. Configure CMake: in this step follow recommendations in `conan install`
   output from the previous step for your version of CMake.

1. Build:

   ```sh
   cd build
   cmake --build . -j
   ```

   *Note*: if you are using multi-config generator (Visual Studio, Ninja Multiconfig, etc), replace the last command with:

   ```sh
   cmake --build . --config Release -j
   ```

## Benchmark

To compare performance between bitonic sort and usual CPU `std::sort()` I have
built benchmark target using Conan profile [bench.profile](profiles/bench.profile).
It contains `-O2 -DNDEBUG` compiler flags
to compare with well optimized CPU version.

All measurements have done on my machine with the following specs:

| Spec | Value |
|:---------:|:--------------:|
| Model | MacBook Pro (2023) |
| OS        |  macOS 15.3 24D60 arm64 |
| Kernel | 24.3.0 |
| CPU | Apple M3 Pro (11-core) |
| GPU | Apple M3 Pro (14-core) |
| RAM (Unified memory) | 18 GB |
| Metal support | Metal 3 |

Results of benchmark can be found in this tabular: [results.csv](results/results.csv).
A single benchmark takes data from a single file from `tests/e2e/generated/input`.
Each benchmark has been run 8 times to filter out possible undesired external impact.
As it can be seen from the table,
bitonic sort is 1.6-2.3 times faster
than `std::sort` in average in
10 tested benchmarks.
