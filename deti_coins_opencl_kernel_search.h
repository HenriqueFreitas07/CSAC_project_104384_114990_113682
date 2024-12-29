#ifndef DETI_COINS_OPENCL_KERNEL_SEARCH_H
#define DETI_COINS_OPENCL_KERNEL_SEARCH_H

#include <CL/cl.h>

static void deti_coins_opencl_kernel_search(void) 
{
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    
    // Initialize variables
    uint32_t v1 = 0x20202020;
    uint32_t v2 = 0x20202020;
    const size_t block_size = 128;
    
    // OpenCL initialization
    // Get platform
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to get platform ID!\n");
        return;
    }
    
    // Get device
    // err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL); // Try any available device (GPU/CPU)

    if (err != CL_SUCCESS) {
        printf("Error: Failed to get device ID!\n");
        return;
    }
    
    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create context!\n");
        return;
    }
    
    // Create command queue
    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create command queue!\n");
        return;
    }
    
    // Create and build program (you'll need to load your OpenCL kernel code here)
    // Note: Replace "kernel_source" with your actual kernel source code
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create program!\n");
        return;
    }
    
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to build program!\n");
        return;
    }
    
    // Create kernel
    kernel = clCreateKernel(program, "md5_opencl_kernel", &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create kernel!\n");
        return;
    }
    
    // Main loop (similar to CUDA version)
    for(n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (1<<20)*64) {
        next_value_to_try(v1);
        if(v1 == 0x20202020) 
            next_value_to_try(v2);
            
        // Create and set buffer arguments
        cl_mem d_v1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(uint32_t), NULL, &err);
        cl_mem d_v2 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(uint32_t), NULL, &err);
        cl_mem d_device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uint32_t), NULL, &err);
        
        // Copy data to device
        err = clEnqueueWriteBuffer(queue, d_v1, CL_TRUE, 0, sizeof(uint32_t), &v1, 0, NULL, NULL);
        err = clEnqueueWriteBuffer(queue, d_v2, CL_TRUE, 0, sizeof(uint32_t), &v2, 0, NULL, NULL);
        
        uint32_t init_val = 1;
        err = clEnqueueWriteBuffer(queue, d_device_data, CL_TRUE, 0, sizeof(uint32_t), &init_val, 0, NULL, NULL);
        
        // Set kernel arguments
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_v1);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_v2);
        clSetKernelArg(kernel, 2, sizeof(uint32_t), &block_size);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), &d_device_data);
        
        // Launch kernel
        size_t global_work_size = (1<<20);
        size_t local_work_size = block_size;
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, 
                                    &local_work_size, 0, NULL, NULL);
        
        // Read results
        uint32_t result;
        clEnqueueReadBuffer(queue, d_device_data, CL_TRUE, 0, sizeof(uint32_t), 
                           &result, 0, NULL, NULL);
        
        if(result >= 32u) {
            save_deti_coin(coin);
            n_coins++;
        }
        
        // Cleanup buffers
        clReleaseMemObject(d_v1);
        clReleaseMemObject(d_v2);
        clReleaseMemObject(d_device_data);
    }
    
    // Cleanup
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    
    STORE_DETI_COINS();
    printf("deti_coins_opencl_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
           n_coins, (n_coins == 1ul) ? "" : "s",
           n_attempts, (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ul << 32));
}

#endif