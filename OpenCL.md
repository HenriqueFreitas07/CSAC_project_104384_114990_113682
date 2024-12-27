### OpenCL vs CUDA

CUDA is NVIDIA's proprietary computing architecture and does not allow for parallel computing unless you are on an NVIDIA-powered chip. OpenCL allows for more parallel computing without requiring the purchase of a specific vendor's card.

```bash
# ====== New Files to Create ======
# md5_opencl_kernel.cl : OpenCL kernel for MD5 hash computation
# deti_coins_opencl_search.h : OpenCL-based DETI coin search logic
# opencl_driver_api_utilities.h : OpenCL host-side utilities for setup and management

# ====== Files to Modify ======
# deti_coins.c : Add OpenCL setup, kernel execution, and validation
# makefile : Add rules to compile and link OpenCL code
```


```bash
# ====== Step 1: Create md5_opencl_kernel.cl ======
# - Implement the OpenCL MD5 kernel
# - Use get_global_id(0) for thread indexing
# - Adapt CUSTOM_MD5_CODE macros for OpenCL
# - Use atomic operations for safe updates

# ====== Step 2: Create deti_coins_opencl_search.h ======
# - Initialize OpenCL buffers
# - Launch OpenCL kernel
# - Read back results from device
# - Validate DETI coins and handle results

# ====== Step 3: Create opencl_driver_api_utilities.h ======
# - Handle OpenCL context, command queue, and kernel initialization
# - Provide error-handling utilities
# - Manage OpenCL memory buffers
# - Include resource cleanup functions

# ====== Step 4: Modify deti_coins.c ======
# - Add OpenCL initialization and cleanup logic
# - Call deti_coins_opencl_search for mining

# Example addition:
# #if USE_OPENCL > 0
#     initialize_opencl("md5_opencl_kernel.cl", "md5_opencl_kernel");
#     deti_coins_opencl_search();
#     terminate_opencl();
# #endif

# ====== Step 5: Update makefile ======
# Add OpenCL build rules:
# OPENCL_FLAGS = -lOpenCL
# md5_opencl: deti_coins.c md5_opencl_kernel.cl
#     gcc deti_coins.c -o md5_opencl $(OPENCL_FLAGS)
```





# OPENCL

Port the MD5 Algorithm to OpenCL C:

OpenCL kernels will need to replicate this macro structure in a way compatible with OpenCL syntax.
Constants, rotations, and state updates must follow the same logical order.

Thread indexing to ensure unique template modifications.

Avoid excessive data transfer between the host and device.

Each OpenCL thread computes MD5 hashes for unique templates.

-   Input buffers: __global uint *data
-   Output buffers: __global uint *hash

__builtin_ia32_pslldi128, vshlq_n_u32 -> need to be replaced with OpenCL equivalents


```bash
n = (u32_t)threadIdx.x + (u32_t)blockDim.x * (u32_t)blockIdx.x;
```
In OpenCL we do:
```bash
n = get_global_id(0);
```

The pointers will be like this:
```bash
interleaved32_data = &interleaved32_data[(n >> 5u) * (32u * 13u) + (n & 31u)];
interleaved32_hash = &interleaved32_hash[(n >> 5u) * (32u *  4u) + (n & 31u)];
```


Now we need to modify deti_coins.c
```bash
#if USE_OPENCL > 0
    initialize_opencl("md5_opencl_kernel.cl", "md5_opencl_kernel");
    deti_coins_opencl_search();
    terminate_opencl();
#endif
```

And the makefile
```bash
# OpenCL Build
OPENCL_FLAGS = -lOpenCL
md5_opencl: deti_coins.c md5_opencl_kernel.cl
    gcc deti_coins.c -o md5_opencl $(OPENCL_FLAGS)
```



TODO: Test with my RTX3070  

```bash
# ====== 1. md5_opencl_kernel.cl ======
# Purpose: OpenCL kernel for MD5 hash computation on GPU.
# Why:
# - CUDA kernels are NVIDIA-specific; OpenCL provides cross-platform GPU support.
# - Each OpenCL work-item computes an MD5 hash for a unique message.
# Details:
# - Uses get_global_id() for thread indexing instead of CUDA's threadIdx/blockIdx.
# - Implements CUSTOM_MD5_CODE for hash computation.
# - Ensures memory-efficient access to __global memory.
# - Uses atomic operations for safe storage of detected DETI coins.

# ====== 2. deti_coins_opencl_search.h ======
# Purpose: OpenCL-based DETI coin search logic.
# Why:
# - The existing deti_coins_cpu_search.h is designed for CPU processing only.
# - A dedicated file is needed to manage OpenCL kernel calls and buffer management.
# Details:
# - Sets up OpenCL buffers (__global memory) for data and hash results.
# - Launches the OpenCL kernel using clEnqueueNDRangeKernel.
# - Handles synchronization and result retrieval using clEnqueueReadBuffer.
# - Implements OpenCL error checking and cleanup.

# ====== 3. opencl_driver_api_utilities.h ======
# Purpose: OpenCL host-side utilities for setup and management.
# Why:
# - CUDA API calls (e.g., cuInit, cuModuleLoad) are incompatible with OpenCL.
# - We need OpenCL-specific equivalents for device, kernel, and buffer management.
# Details:
# - Initializes OpenCL context, command queue, and kernel.
# - Manages memory buffers (clCreateBuffer, clReleaseMemObject).
# - Provides error-handling macros and resource cleanup routines.

# ====== 4. deti_coins.c ======
# Purpose: Integrate OpenCL into the main mining program.
# Why:
# - Existing code supports CPU (md5_cpu) and CUDA (md5_cuda_kernel).
# - We need a conditional compilation block for OpenCL execution.
# Modifications:
# - Add a section controlled by #if USE_OPENCL > 0.
# - Call initialize_opencl() to set up the OpenCL environment.
# - Call deti_coins_opencl_search() to start mining.
# - Add terminate_opencl() for cleanup.
# Example Addition:
# #if USE_OPENCL > 0
#     initialize_opencl("md5_opencl_kernel.cl", "md5_opencl_kernel");
#     deti_coins_opencl_search();
#     terminate_opencl();
# #endif

# ====== 5. makefile ======
# Purpose: Add build rules for OpenCL integration.
# Why:
# - Existing rules only compile CPU and CUDA code.
# - OpenCL requires linking with OpenCL libraries and proper flags.
# Modifications:
# - Add an OpenCL-specific build rule:
# OPENCL_FLAGS = -lOpenCL
# md5_opencl: deti_coins.c md5_opencl_kernel.cl
#     gcc deti_coins.c -o md5_opencl $(OPENCL_FLAGS)
# - Ensure conditional compilation is properly handled with USE_OPENCL.
```

Why These Changes Are Necessary
Cross-Platform Support:

CUDA is limited to NVIDIA GPUs.
OpenCL supports CPUs, GPUs (NVIDIA, AMD), and other accelerators.
Kernel Differences:

OpenCL uses get_global_id() for thread indexing, unlike CUDA’s threadIdx and blockIdx.
OpenCL memory management (clCreateBuffer) differs from CUDA (cuMemAlloc).
Host-Side Integration:

opencl_driver_api_utilities.h abstracts OpenCL API calls for easier usage.
deti_coins_opencl_search.h manages search logic and ensures synchronization.
Conditional Compilation:

Changes in deti_coins.c allow seamless toggling between CPU, CUDA, and OpenCL backends.
makefile ensures proper compilation and linking with OpenCL libraries.