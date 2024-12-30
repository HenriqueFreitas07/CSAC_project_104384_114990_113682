// deti_coins_opencl_kernel_search.h

#ifndef DETI_COINS_OPENCL_KERNEL_SEARCH
#define DETI_COINS_OPENCL_KERNEL_SEARCH

#include <CL/cl.h>
#include "md5.h"

static void deti_coins_opencl_kernel_search(void)
{
    u32_t v1 = 0x20202020;
    u32_t v2 = 0x20202020;
    u32_t block_size = 128;

    // Initialize OpenCL
    initialize_opencl(0, "md5_opencl_kernel.cl", "opencl_md5_kernel", 0, 1024);

    for (n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (1 << 20) * 64)
    {
        next_value_to_try(v1);
        if (v1 == 0x20202020)
            next_value_to_try(v2);

        // Set kernel arguments
        CL_CALL(clSetKernelArg(cl_kernel, 0, sizeof(u32_t), &v1));
        CL_CALL(clSetKernelArg(cl_kernel, 1, sizeof(u32_t), &v2));
        CL_CALL(clSetKernelArg(cl_kernel, 2, sizeof(u32_t), &device_data));

        // Set initial device data
        device_data[0] = 1;

        // Launch OpenCL kernel
        size_t global_size = (1 << 20);
        size_t local_size = block_size;
        CL_CALL(clEnqueueNDRangeKernel(cl_command_queue, cl_kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL));

        // Read results from the device
        CL_CALL(clEnqueueReadBuffer(cl_command_queue, device_data_buffer, CL_TRUE, 0, sizeof(u32_t) * 1024, device_data, 0, NULL, NULL));

        if (device_data[0] >= 32u)
        {
            save_deti_coin(coin);
            n_coins++;
        }
    }

    terminate_opencl();
    STORE_DETI_COINS();
    printf("deti_coins_opencl_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
           n_coins, (n_coins == 1ul) ? "" : "s",
           n_attempts, (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ul << 32));
}

#endif
