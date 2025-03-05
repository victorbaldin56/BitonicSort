#include <cstdlib>

#include "cl_sort/cl_bitonic_sort.hh"

int main() try {
  auto n = std::size_t{};
  std::cin.exceptions(std::ios::failbit | std::ios::eofbit);
  std::cin >> n;

  auto data = std::vector<int>(n);
  data.reserve(n);
  while (data.size() < n) {
    auto e = 0;
    std::cin >> e;
    data.push_back(e);
  }

  // sorting with OpenCL
  ocl::BitonicSorter app;
  app.sort(data);
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  return 0;
} catch (std::exception& ex) {
  std::cerr << ex.what() << std::endl;
  return EXIT_FAILURE;
}
