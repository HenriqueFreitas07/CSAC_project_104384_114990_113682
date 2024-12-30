// deti_coins_opencl_search.h

#ifndef DETI_COINS_OPENCL_SEARCH
#define DETI_COINS_OPENCL_SEARCH

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "deti_coins_vault.h" // we are obliged to include it, it doesn't transfer


extern int stop_request;  // Declare stop_request as an external variable

typedef uint32_t u32_t;
typedef uint64_t u64_t;

long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

u32_t next_value_to_try_ascii(u32_t v) {
    v++;
    if ((v & 0xFF) == 0x7F) {
        v += 0xA1;
        if (((v >> 8) & 0xFF) == 0x7F) {
            v += 0xA1 << 8;
            if (((v >> 16) & 0xFF) == 0x7F) {
                v += 0xA1 << 16;
                if (((v >> 24) & 0xFF) == 0x7F)
                    v += 0xA1 << 24;
            }
        }
    }
    return v;
}

void deti_coins_opencl_search(u32_t random_words) {
    u32_t block_size = 128;
    struct timespec start_time, end_time;
    u32_t size = 1024u;
    u64_t n_attempts = 0, n_coins = 0, kernel_calls = 0, sum_times_ns = 0;
    u32_t idx, max_idx, custom_word1, custom_word2;
    custom_word1 = custom_word2 = 0x20202020;

    initialize_opencl(0, "md5_opencl_kernel.cl", "opencl_md5_kernel", size, 0u);
    max_idx = 1u;

    for (n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (64ul << 20ul)) {
        host_data[0] = 1u;
        CL_CALL(clEnqueueWriteBuffer, (command_queue, device_data, CL_TRUE, 0, size * sizeof(u32_t), host_data, 0, NULL, NULL));

        // Set kernel arguments
        CL_CALL(clSetKernelArg, (kernel, 0, sizeof(u32_t), &custom_word1));
        CL_CALL(clSetKernelArg, (kernel, 1, sizeof(u32_t), &custom_word2));
        CL_CALL(clSetKernelArg, (kernel, 2, sizeof(cl_mem), &device_data));

        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // Launch OpenCL kernel
        size_t global_size = (1 << 20);
        size_t local_size = block_size;
        CL_CALL(clEnqueueNDRangeKernel, (command_queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL));

        clFinish(command_queue);
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        CL_CALL(clEnqueueReadBuffer, (command_queue, device_data, CL_TRUE, 0, size * sizeof(u32_t), host_data, 0, NULL, NULL));

        if (host_data[0] > max_idx)
            max_idx = host_data[0];

        for (idx = 1u; idx < host_data[0] && idx <= size - 13u; idx += 13) {
            if (idx <= size - 13) {
                save_deti_coin(&host_data[idx]);
                n_coins++;
            } else {
                fprintf(stderr, "deti_coins_opencl_search: wasted DETI coin\n");
            }
        }

        if (custom_word1 != 0x7E7E7E7Eu)
            custom_word1 = next_value_to_try_ascii(custom_word1);
        else {
            custom_word1 = 0x20202020u;
            custom_word2 = next_value_to_try_ascii(custom_word2);
        }

        long elapsed_time_ns = calculate_elapsed_time(start_time, end_time);
        sum_times_ns += elapsed_time_ns;
        kernel_calls++;
    }

    // Calculate average kernel execution time in microseconds
    double avg_time_us = (double)sum_times_ns / (kernel_calls * 1e3);
    printf("Average kernel execution time: %.3f µs\n", avg_time_us);

    STORE_DETI_COINS();
    printf("deti_coins_opencl_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
           n_coins,
           (n_coins == 1ul) ? "" : "s",
           n_attempts,
           (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ul << 32));

    terminate_opencl();
}

#endif // DETI_COINS_OPENCL_SEARCH
