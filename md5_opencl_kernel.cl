// md5_opencl_kernel.cl


typedef unsigned int u32_t;
#include <md5.h>

#define next_value_to_try(v)                              \
    do                                                    \
    {                                                    \
        v++;                                             \
        if ((v & 0xFFu) == 0x7Fu)                        \
        {                                                \
            v += 0xA1u;                                  \
            if (((v >> 8) & 0xFFu) == 0x7Fu)             \
            {                                            \
                v += 0xA1u << 8;                         \
                if (((v >> 16) & 0xFFu) == 0x7Fu)        \
                {                                        \
                    v += 0xA1u << 16;                    \
                    if (((v >> 24) & 0xFFu) == 0x7Fu)    \
                        v += 0xA1u << 24;                \
                }                                        \
            }                                            \
        }                                                \
    } while (0)

__kernel void md5_opencl_kernel(
    const u32_t v1,
    const u32_t v2,
    __global u32_t *data_storage_device)
{
    u32_t number_coins = 64u;
    u32_t n, a, b, c, d, state[4], x[16], coin[13u], hash[4u];
    u32_t previous;
    
    n = get_global_id(0);
    
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

    for (n = 0u; n < number_coins; n++) {
#define C(c)         (c)
#define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
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

        u32_t idx = data_storage_device[0];
        
        if (hash[3] == 0 && idx < 1024 - 13) {
            idx = atomic_add(&data_storage_device[0], 13); // Atomic index increment
            printf("Coin: %52.52s", (char *)coin);
            for (int j = 0; j < 13; j++) {
                data_storage_device[idx + j] = coin[j]; // Copy coin to storage
            }
        }
        
        next_value_to_try(coin[9u]);
        for (u32_t offset = 10u; offset < 13u; offset++) {
            previous = coin[offset - 1u];
            if (previous == 0x7E7E7E7Eu) {
                next_value_to_try(coin[offset]);
            }
        }
    }
}
