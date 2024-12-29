#include "md5.h"

__kernel void opencl_md5_kernel(
    const uint v1,  // Custom word 1
    const uint v2,  // Custom word 2
    __global uint* data_storage_device)  // Shared buffer for results
{
    const uint number_coins = 64u;  
    uint n, state[4], x[16], coin[13], hash[4], idx;
    n = get_global_id(0);  // Get global thread index

    uint previous;

    // Mandatory for a DETI coin
    coin[0u] = 0x49544544; // ITED
    coin[1u] = 0x696f6320; // ioc_ 
    coin[2u] = 0x7343206e; // sC_n
    coin[3u] = 0x30324341; // 02CA
    coin[4u] = 0x41203432; // A_42
    coin[5u] = 0x34314441; // 41DA
    coin[6u] = v1; 
    coin[7u] = v2; 

    // Insert complexity with the thread ID
    coin[8u] = 0x20202020; 
    coin[8u] += (n % 64u) << 0; n /= 64u;
    coin[8u] += (n % 64u) << 8; n /= 64u;
    coin[8u] += (n % 64u) << 16; n /= 64u;
    coin[8u] += (n % 64u) << 24; n /= 64u;
    coin[9u] = 0x20202020; 
    coin[10u] = 0x20202020; 
    coin[11u] = 0x20202020; 
    coin[12u] = 0x0A202020; 

    printf("%s\n", "it arrived here");

    for (n = 0u; n < number_coins; n++) {

        // this shouldnt be here
        uint a, b, c, d;

        // MD5 computation (main logic)
        #define C(c)         (c)
        #define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
        #define DATA(idx)    coin[idx] 
        #define HASH(idx)    hash[idx]
        #define STATE(idx)   state[idx]
        #define X(idx)       x[idx]

        CUSTOM_MD5_CODE();  // Perform MD5 transformation on the coin
        
        #undef C
        #undef ROTATE
        #undef DATA
        #undef HASH
        #undef STATE
        #undef X
        
        // Check if the hash meets the criteria and store the coin in global storage
        idx = atomic_add(&data_storage_device[0], 13); // Reserve space atomically
        if (idx < 1024 - 13) {
            for (int j = 0; j < 13; j++) {
                data_storage_device[idx + j] = coin[j];
            }
        }

        // Increment coin values for the next try
        for (uint offset = 8u; offset < 13u; offset++) {
            coin[offset]++;
            if (coin[offset] != 0) {
                break; // Stop if no overflow happened
            }
        }
    }
}
