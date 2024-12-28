// md5_opencl_kernel.cl
__kernel void md5_opencl_kernel(
    const u32_t v1,
    const u32_t v2,
    __global u32_t* data_storage_device)
{
    const u32_t number_coins = 64u;
    u32_t n, state[4], x[16], coin[13], hash[4];
    
    // Get global ID
    n = get_global_id(0);
    
    // Initialize coin data
    coin[0] = 0x49544544; // ITED
    coin[1] = 0x696f6320; // ioc_
    coin[2] = 0x7343206e; // sC_n
    coin[3] = 0x30324341; // 02CA
    coin[4] = 0x41203432; // A_42
    coin[5] = 0x34314441; // 41DA
    coin[6] = v1;
    coin[7] = v2;
    
    // Thread complexity
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
        // MD5 computation
        #define C(c)         (c)
        #define ROTATE(x,n)  rotate((u32_t)(x), (u32_t)(n))
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

        // Check and store coin
        u32_t idx = atomic_add(&data_storage_device[0], 0); // Read current index
        if (hash[3] == 0 && idx < 1024 - 13) {
            idx = atomic_add(&data_storage_device[0], 13);
            if (idx < 1024 - 13) {
                for (int j = 0; j < 13; j++) {
                    data_storage_device[idx + j] = coin[j];
                }
            }
        }

        // Update values
        for (u32_t offset = 9u; offset < 13u; offset++) {
            u32_t previous = coin[offset-1u];
            if (previous == 0x7F7F7F7Fu) {
                next_value_to_try(coin[offset]);
            }
            next_value_to_try(coin[offset]);
        }
    }
}