/**
 * Bitonic sort implementation.
 */
__kernel void bitonicSort(__global TYPE *data, __local TYPE *local_data,
                          const unsigned sz) {
  unsigned int i = get_local_id(0);
  unsigned int global_i = get_global_id(0);

  local_data[i] = data[global_i];
  barrier(CLK_LOCAL_MEM_FENCE);

  for (unsigned int stage = 1; stage < sz; stage <<= 1) {
    for (unsigned int step = stage; step > 0; step >>= 1) {
      unsigned int pair_idx = i ^ step;
      if (pair_idx > i) {
        bool ascending = ((i & stage) == 0);
        TYPE a = local_data[i];
        TYPE b = local_data[pair_idx];
        if ((ascending && a > b) || (!ascending && a < b)) {
          local_data[i] = b;
          local_data[pair_idx] = a;
        }
      }
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }
  data[global_i] = local_data[i];
}
