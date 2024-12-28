__kernel void opencl_md5_kernel(u32_t v1, u32_t v2, __global u32_t* data_storage_device)
{
    u32_t n = get_global_id(0);  // Get global thread ID
    u32_t coin[13] = {0};
    u32_t hash[4];
    u32_t previous;

    // Initialize coin data (same as CUDA)
    coin[0] = 0x49544544;  // ITED
    coin[1] = 0x696f6320;  // ioc_
    coin[2] = 0x7343206e;  // sC_n
    coin[3] = 0x30324341;  // 02CA
    coin[4] = 0x41203432;  // A_42
    coin[5] = 0x34314441;  // 41DA
    coin[6] = v1;
    coin[7] = v2;
    coin[8] = 0x20202020 + (n % 64);  // Thread ID as part of the coin
    coin[9] = 0x20202020;
    coin[10] = 0x20202020;
    coin[11] = 0x20202020;
    coin[12] = 0x0A202020;

    // Perform MD5 operation (custom code goes here)
    CUSTOM_MD5_CODE();  // Implement MD5 logic

    // Check for match and update storage
    u32_t idx = data_storage_device[0];
    if (hash[3] == 0 && idx < 1024 - 13) {
        idx = atomic_add(&data_storage_device[0], 13);
        for (int i = 0; i < 13; i++) {
            data_storage_device[idx + i] = coin[i];
        }
    }

    // Update coin values for next attempt
    for (u32_t offset = 9; offset < 13; offset++) {
        previous = coin[offset - 1];
        if (previous == 0x7F7F7F7F) {
            next_value_to_try(coin[offset]);
        }
        next_value_to_try(coin[offset]);
    }
}
