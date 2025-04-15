/**
 * Bitonic sort OpenCL implementation.
 */

#include "cl_sort/cl_bitonic_sort.hh"

#include <cassert>
#include <numeric>

namespace {

constexpr std::size_t kMaxLocalSize = 0x100;

template <typename Integer,
          typename = std::enable_if_t<std::is_unsigned_v<Integer>>>
constexpr auto nextPowerOfTwo(Integer n) {
  --n;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  if (sizeof(n) > 1) {
    n |= n >> 8;
  }
  if (sizeof(n) > 2) {
    n |= n >> 16;
  }
  if (sizeof(n) > 4) {
    n |= n >> 32;
  }
  ++n;
  return n;
}

/**
 * Resize vector to closest power of two
 */
void prepareData(std::vector<int>& data) {
  auto old_sz = data.size();
  auto new_sz = nextPowerOfTwo(old_sz);
  data.resize(new_sz, std::numeric_limits<int>::max());
}

}  // namespace

namespace cl_sort {

BitonicSorter::BitonicSorter(const cl_app::Config& cfg)
    : app_(std::filesystem::absolute(__FILE__)
               .parent_path()
               .append("shaders")
               .append("bitonic_sort.cl"),
           cfg) {}

void BitonicSorter::sort(std::vector<int>& data) const {
  auto old_data_sz = data.size();
  prepareData(data);
  sortImpl(data, old_data_sz);
  data.resize(old_data_sz);
}

void BitonicSorter::sortImpl(std::vector<int>& data,
                             std::size_t orig_size) const {
  auto sz = data.size();
  assert((sz & (sz - 1)) == 0);

  auto global_size = sz;
  auto local_size = std::min(global_size, kMaxLocalSize);

  auto buffer = app_.allocateBuffer(CL_MEM_READ_WRITE, sizeof(int) * sz);
  app_.copy(data.begin(), data.end(), buffer);

  auto local =
      static_cast<cl::LocalSpaceArg>(cl::Local(local_size * sizeof(int)));

  auto bitonic_split = app_.getKernel("bitonicSplit");
  auto bitonic_merge = app_.getKernel("bitonicMerge");

  bitonic_split.setArg(0, buffer);
  bitonic_split.setArg(1, local);

  auto events = std::vector<cl::Event>();
  app_.runKernel(bitonic_split, global_size, local_size, events);
  events.front().wait();

  events.clear();

  for (auto stage = std::size_t{2}; stage <= global_size; stage <<= 1) {
    for (auto step = stage >> 1; step > 0; step >>= 1) {
      bitonic_merge.setArg(0, buffer);
      bitonic_merge.setArg(1, stage);
      bitonic_merge.setArg(2, step);

      app_.runKernel(bitonic_merge, global_size, local_size, events);
    }
  }

  std::for_each(events.begin(), events.end(), [](auto& evt) { evt.wait(); });

  app_.copy(buffer, data.begin(), data.begin() + orig_size);
}

}  // namespace cl_sort
