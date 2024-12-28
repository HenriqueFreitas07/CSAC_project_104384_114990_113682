// OpenCL MD5 kernel implementation

typedef unsigned int u32_t;

#include "md5.h"

// Macro for value generation (kept the same as original)
#define next_value_to_try(v)                              \
    do                                                    \
    {                                                     \
        v++;                                              \
        if ((v & 0xFFu) == 0x7Fu)                        \
        {                                                 \
            v += 0xA1u;                                   \
            if (((v >> 8) & 0xFFu) == 0x7Fu)             \
            {                                             \
                v += 0xA1u << 8;                          \
                if (((v >> 16) & 0xFFu) == 0x7Fu)        \
                {                                         \
                    v += 0xA1u << 16;                     \
                    if (((v >> 24) & 0xFFu) == 0x7Fu)    \
                        v += 0xA1u << 24;                 \
                }                                         \
            }                                             \
        }                                                 \
    } while(0)

// OpenCL kernel function
__kernel void md5_opencl_kernel(
    const u32_t v1,
    const u32_t v2,
    __global u32_t* data_storage_device)
{
    const u32_t number_coins = 64u;
    u32_t n, a, b, c, d, state[4], x[16], coin[13], hash[4];
    
    // Get global ID (equivalent to CUDA's thread calculation)
    n = get_global_id(0);
    
    // Mandatory DETI coin initialization
    coin[0] = 0x49544544; // ITED
    coin[1] = 0x696f6320; // ioc_
    coin[2] = 0x7343206e; // sC_n
    coin[3] = 0x30324341; // 02CA
    coin[4] = 0x41203432; // A_42
    coin[5] = 0x34314441; // 41DA
    coin[6] = v1;
    coin[7] = v2;
    
    // Insert complexity with thread ID
    coin[8] = 0x20202020;
    coin[8] += (n % 64u) << 0; n /= 64u;
    coin[8] += (n % 64u) << 8; n /= 64u;
    coin[8] += (n % 64u) << 16; n /= 64u;
    coin[8] += (n % 64u) << 24; n /= 64u;
    coin[9] = 0x20202020;
    coin[10] = 0x20202020;
    coin[11] = 0x20202020;
    coin[12] = 0x0A202020;

    for (n = 0u; n < number_coins; n++) {
        // MD5 hash computation
        #define C(c)         (c)
        #define ROTATE(x,n)  (rotate((u32_t)(x), (u32_t)(n)))
        #define DATA(idx)    coin[idx]
        #define HASH(idx)    hash[idx]
        #define STATE(idx)   state[idx]
        #define X(idx)       x[idx]
        
        CUSTOM_MD5_CODE();
        
        #undef C
        #undef ROTATE
        #undef DATA
        #undef HASH
        #undef STATE
        #undef X

        // Check for new coin and store if found
        u32_t idx = data_storage_device[0];
        if (hash[3] == 0 && idx < 1024 - 13) {
            // Atomic increment (OpenCL equivalent of CUDA's atomicAdd)
            idx = atomic_add(&data_storage_device[0], 13);
            
            // Store the coin if we found one
            if (idx < 1024 - 13) {
                for (int j = 0; j < 13; j++) {
                    data_storage_device[idx + j] = coin[j];
                }
                // Debug output (note: OpenCL printf is implementation-dependent)
                printf("Coin: %52.52s", (char *)coin);
            }
        }

        // Update coin values
        for (u32_t offset = 9u; offset < 13u; offset++) {
            u32_t previous = coin[offset-1u];
            if (previous == 0x7F7F7F7Fu) {
                next_value_to_try(coin[offset]);
            }
            next_value_to_try(coin[offset]);
        }
    }
}