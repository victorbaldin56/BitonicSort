#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "cl_app/cl_app.hh"

namespace cl_sort {

class BitonicSorter final {
 public:
  BitonicSorter(const cl_app::Config& cfg = cl_app::readConfig());
  void sort(std::vector<int>& data) const;

 private:
  void sortImpl(std::vector<int>& data, std::size_t orig_size) const;

 private:
  cl_app::ClApplication app_;
};

}  // namespace cl_sort
