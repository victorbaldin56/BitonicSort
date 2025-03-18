#include <cstdlib>

#include "cl_sort/cl_bitonic_sort.hh"

int main() try {
  auto n = std::size_t{};
  std::cin.exceptions(std::ios::failbit);
  std::cin >> n;
  auto data = std::vector<int>();
  data.reserve(n);
  std::copy_n(std::istream_iterator<int>(std::cin),
              n, std::back_inserter(data));

  bts::BitonicSorter app;
  app.sort(data);
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  return 0;
} catch (std::exception& ex) {
  std::cerr << ex.what() << std::endl;
  return EXIT_FAILURE;
}
