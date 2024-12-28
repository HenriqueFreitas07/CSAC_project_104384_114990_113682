#ifndef DETI_COINS_OPENCL_SEARCH
#define DETI_COINS_OPENCL_SEARCH

#include <CL/cl.h>

// Keep the ASCII value generation function unchanged as it's platform-independent
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
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    
    const u32_t block_size = 128;
    const u32_t size = 1024u;
    
    // Initialize OpenCL
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to get platform ID!\n");
        return;
    }
    
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to get device ID!\n");
        return;
    }
    
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to create context!\n");
        return;
    }
    
    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to create command queue!\n");
        return;
    }
    
    // Create program from source (you'll need to provide the kernel source)
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to create program!\n");
        return;
    }
    
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to build program!\n");
        return;
    }
    
    kernel = clCreateKernel(program, "md5_opencl_kernel", &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to create kernel!\n");
        return;
    }
    
    // Initialize variables
    u32_t n_attempts = 0, n_coins = 0;
    u32_t idx, max_idx, custom_word1, custom_word2;
    custom_word1 = custom_word2 = 0x20202020;
    max_idx = 1u;
    
    // Create buffers
    cl_mem device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, 
                                      size * sizeof(u32_t), NULL, &err);
    u32_t *host_data = (u32_t *)malloc(size * sizeof(u32_t));
    
    for(n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (1<<20)*64u) {
        host_data[0] = 1u;
        
        // Copy data to device
        err = clEnqueueWriteBuffer(queue, device_data, CL_TRUE, 0,
                                 size * sizeof(u32_t), host_data, 0, NULL, NULL);
        
        // Set kernel arguments
        clSetKernelArg(kernel, 0, sizeof(u32_t), &custom_word1);
        clSetKernelArg(kernel, 1, sizeof(u32_t), &custom_word2);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &device_data);
        
        // Launch kernel
        size_t global_work_size = (1 << 20);
        size_t local_work_size = block_size;
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
                                    &global_work_size, &local_work_size,
                                    0, NULL, NULL);
        
        // Copy results back
        err = clEnqueueReadBuffer(queue, device_data, CL_TRUE, 0,
                                size * sizeof(u32_t), host_data,
                                0, NULL, NULL);
        
        if(host_data[0] > max_idx)
            max_idx = host_data[0];
            
        // Collect generated coins
        for(idx = 1u; idx < host_data[0] && idx <= size - 13u; idx += 13) {
            if(idx <= size - 13) {
                save_deti_coin(&host_data[idx]);
                n_coins++;
            } else {
                fprintf(stderr, "deti_coins_opencl_search: wasted DETI coin\n");
            }
        }
        
        if(custom_word1 != 0x7E7E7E7Eu)
            custom_word1 = next_value_to_try_ascii(custom_word1);
        else {
            custom_word1 = 0x20202020u;
            custom_word2 = next_value_to_try_ascii(custom_word2);
        }
    }
    
    // Cleanup
    free(host_data);
    clReleaseMemObject(device_data);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    
    STORE_DETI_COINS();
    printf("deti_coins_opencl_search: %u DETI coin%s found in %u attempt%s (expected %.2f coins)\n",
           n_coins,
           (n_coins == 1ul) ? "" : "s",
           n_attempts,
           (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ul << 32));
}

#endif // DETI_COINS_OPENCL_SEARCH