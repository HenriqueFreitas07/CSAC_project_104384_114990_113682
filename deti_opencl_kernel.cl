// #include <stdio.h>
// #include <stdlib.h>
// #include <CL/cl.h>
// #include "deti_opencl.h"

// // OpenCL mining function
// void deti_opencl_opencl(unsigned int seconds)
// {
//     cl_int ret;
//     cl_platform_id platform;
//     cl_device_id device;
//     cl_context context;
//     cl_command_queue queue;
//     cl_kernel kernel;
//     cl_program program;

//     // Get OpenCL platform and device
//     ret = clGetPlatformIDs(1, &platform, NULL);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to get platform!\n");
//         return;
//     }
//     ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to get device!\n");
//         return;
//     }

//     // Create OpenCL context and queue
//     context = clCreateContext(NULL, 1, &device, NULL, NULL, &ret);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to create context!\n");
//         return;
//     }
//     queue = clCreateCommandQueue(context, device, 0, &ret);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to create command queue!\n");
//         return;
//     }

//     // Load and compile OpenCL kernel
//     const char *kernel_source = load_kernel_source("deti_opencl_kernel.cl");
//     program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &ret);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to create program!\n");
//         return;
//     }
//     ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to build program!\n");
//         return;
//     }
//     kernel = clCreateKernel(program, "deti_opencl_kernel", &ret);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to create kernel!\n");
//         return;
//     }

//     // Set kernel arguments, memory buffers, and enqueue execution
//     cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to create buffer!\n");
//         return;
//     }

//     ret = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &seconds, NULL, 0, NULL, NULL);
//     if (ret != CL_SUCCESS) {
//         printf("Error: Failed to enqueue kernel!\n");
//         return;
//     }

//     // Wait for kernel execution to finish
//     clFinish(queue);

//     // Clean up OpenCL resources
//     clReleaseMemObject(buffer);
//     clReleaseKernel(kernel);
//     clReleaseProgram(program);
//     clReleaseCommandQueue(queue);
//     clReleaseContext(context);

//     printf("Mining completed.\n");
// }
__kernel void md5_kernel(__global uint *coins, __global uint *hashes) {
    int id = get_global_id(0);

    // Predefined MD5 constants
    uint T[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 
                  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0x00000000,
                  0x2b6d7b9f, 0xd8e3d6d3, 0x36b7889b, 0x9c0a85c8, 0x23b2644d, 0xbdf7d938, 0x835ff00f, 0x11e65a97, 0xd88ecf9a, 
                  0xc8e99f79, 0x8cc8b093, 0xc6cdb27c, 0x050c5c32, 0xc9098409, 0x763173a3, 0xf8c8c853, 0x365a86cc, 0x80bffb47,
                  0x94b4a732, 0xe4d5a1d5, 0x34ffec5f, 0x4770b2ca, 0x9471e577, 0x1a2b4299, 0xa71f0d2b, 0x74ec6b12, 0x3c3a2b62, 
                  0x9e800b09, 0x1dbed3a7, 0x57e11b88, 0xfbd4c9ff, 0x8a4e0505, 0xe17eb3f3, 0x4f5b8c2d, 0xa25de52b, 0x6f287e24};
    
    uint A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0x10325476;
    uint temp;
    int i;

    // Load the message in blocks of 64 bytes (16 32-bit words)
    uint block[16];
    for (i = 0; i < 16; i++) {
        block[i] = coins[id * 16 + i];  // coins are passed in as an array of uints
    }

    // MD5 rounds, using the constants T[] for each round
    for (i = 0; i < 64; i++) {
        if (i < 16) {
            temp = (B & C) | (~B & D);
        } else if (i < 32) {
            temp = (B & D) | (C & ~D);
        } else if (i < 48) {
            temp = B ^ C ^ D;
        } else {
            temp = C ^ (B | ~D);
        }
        
        temp = temp + A + T[i] + block[i % 16];
        A = D;
        D = C;
        C = B;
        B = B + ((temp << 7) | (temp >> (32 - 7))); // Left rotate
    }

    // Store the resulting hash
    hashes[id * 4] = A;
    hashes[id * 4 + 1] = B;
    hashes[id * 4 + 2] = C;
    hashes[id * 4 + 3] = D;
}
