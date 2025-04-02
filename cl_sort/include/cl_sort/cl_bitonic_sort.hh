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
  BitonicSorter();
  void sort(std::vector<int>& data) const;

 private:
  cl_app::ClApplication app_;
};

}  // namespace cl_sort
