__kernel void md5_opencl_kernel(__global uint *interleaved32_data,
                                __global uint *interleaved32_hash) {
    uint n = get_global_id(0);
    uint a, b, c, d, state[4], x[16];
    
    interleaved32_data += (n >> 5) * (32 * 13) + (n & 31);
    interleaved32_hash += (n >> 5) * (32 * 4) + (n & 31);

    #define C(c)         (c)
    #define ROTATE(x,n)  rotate(x,n)
    #define DATA(idx)    interleaved32_data[32 * (idx)]
    #define HASH(idx)    interleaved32_hash[32 * (idx)]
    #define STATE(idx)   state[idx]
    #define X(idx)       x[idx]

    CUSTOM_MD5_CODE();

    #undef C
    #undef ROTATE
    #undef DATA
    #undef HASH
    #undef STATE
    #undef X
}


// Kernel Design Overview
// Input Buffers: __global uint *interleaved32_data
// Output Buffers: __global uint *interleaved32_hash
// Thread Indexing: Use get_global_id(0) to uniquely identify work-items.
// MD5 Logic: Translate CUSTOM_MD5_CODE macros using OpenCL syntax (rotate, __global, etc.).
// Atomic Operations: Use atomic_inc() or atomic_store() for safe updates to shared buffers.
