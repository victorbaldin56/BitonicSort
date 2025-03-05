/**
 * @file bitonic_sort.cl
 * Bitonic sort kernels.
 */

/**
 * Bitonic split
 */
__kernel void bitonicSplit(__global int* data,
                           __local int* local_data) {
  size_t gid = get_global_id(0);
  size_t lid = get_local_id(0);
  size_t group_id = get_group_id(0);
  size_t local_size = get_local_size(0);

  bool descending = (group_id & 1);

  local_data[lid] = data[gid];
  barrier(CLK_LOCAL_MEM_FENCE);

  for (size_t stage = 2; stage <= local_size; stage <<= 1) {
    for (size_t step = stage >> 1; step > 0; step >>= 1) {
      size_t pair_idx = lid ^ step;
      if (pair_idx > lid) {
        //descending = !(lid & stage);
        if (descending) {
          if (local_data[lid] < local_data[pair_idx]) {
            int tmp = local_data[lid];
            local_data[lid] = local_data[pair_idx];
            local_data[pair_idx] = tmp;
          }
        }
        else {
          if (local_data[lid] > local_data[pair_idx]) {
            int tmp = local_data[lid];
            local_data[lid] = local_data[pair_idx];
            local_data[pair_idx] = tmp;
          }
        }
      }
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }

  data[gid] = local_data[lid];
}

/**
 * Bitonic merge
 */
__kernel void bitonicMerge(__global int* data,
                           const ulong stage,
                           const ulong step) {
  size_t gid = get_global_id(0);
  size_t pair_idx = gid ^ step;
  if (pair_idx > gid) {
    if ((gid & stage) == 0) {
      if (data[gid] > data[pair_idx]) {
        int tmp = data[gid];
        data[gid] = data[pair_idx];
        data[pair_idx] = tmp;
      }
    }
    else {
      if (data[gid] < data[pair_idx]) {
        int tmp = data[gid];
        data[gid] = data[pair_idx];
        data[pair_idx] = tmp;
      }
    }
  }
}
