typedef unsigned int u32_t;

#include "md5.h"

//
// the nvcc compiler stores x[] and state[] in registers (constant indices!)
//
// global thread number: n = threadIdx.x + blockDim.x * blockIdx.x
// global warp number: n >> 5
// warp thread number: n & 31

extern "C" __global__ __launch_bounds__(128,1) void deti_coins_cuda_