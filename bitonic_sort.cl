/**
 * Bitonic sort implementation.
 */
#define TYPE int

__kernel void bitonicSort(__global TYPE *data, __local TYPE *local_data,
                          const ulong sz) {
  size_t lid = get_local_id(0);
  size_t gid = get_global_id(0);

  local_data[lid] = data[gid];
  barrier(CLK_LOCAL_MEM_FENCE);

  for (size_t stage = 2; stage <= sz; stage <<= 1) {
#pragma unroll
    for (size_t step = stage >> 1; step > 0; step >>= 1) {
      size_t pair_idx = lid ^ step;
      if (pair_idx > lid) {
        bool ascending = ((lid & stage) == 0);
        TYPE a = local_data[lid];
        TYPE b = local_data[pair_idx];
        if ((ascending && a > b) || (!ascending && a < b)) {
          local_data[lid] = b;
          local_data[pair_idx] = a;
        }
      }
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }
  data[gid] = local_data[lid];
}
