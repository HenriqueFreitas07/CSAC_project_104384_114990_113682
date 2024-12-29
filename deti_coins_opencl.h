#ifndef DETI_COINS_OPENCL_H
#define DETI_COINS_OPENCL_H

#define CL_TARGET_OPENCL_VERSION 300

#include <CL/cl.h>
#include "md5.h"
#include <stdint.h>  // For uint32_t and other standard integer types
#include <stdlib.h>  // For malloc and free
#include <stdio.h>   // For fprintf and printf
#include "deti_coins_vault.h"

// Define u32_t if not already defined
typedef uint32_t u32_t;

// Declare stop_request globally or pass as an argument
static volatile int stop_request = 0;  // Declare as global flag

// OpenCL context globals
static cl_context context = NULL;
static cl_command_queue queue = NULL;
static cl_kernel kernel = NULL;
static cl_program program = NULL;
static cl_device_id device = NULL;
static cl_mem device_data = NULL;

// Declare cleanup_opencl function before its usage
static void cleanup_opencl(void);  // Declaration of the cleanup function

// Value generation function used across components
static u32_t next_value_to_try_ascii(u32_t v) {
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

// OpenCL initialization function
static void initialize_opencl(const char* kernel_file_path) {
    cl_int err;
    cl_platform_id platform;

    // Get platform
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to get OpenCL platform\n");
        exit(1);
    }

    // Get device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to get GPU device\n");
        exit(1);
    }

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to create context\n");
        exit(1);
    }

    // Create command queue
    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to create command queue\n");
        exit(1);
    }

    // Load kernel source code from file
    FILE *file = fopen(kernel_file_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open kernel file: %s\n", kernel_file_path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t kernel_size = ftell(file);
    rewind(file);

    char *kernel_source = (char *)malloc(kernel_size + 1);
    fread(kernel_source, 1, kernel_size, file);
    kernel_source[kernel_size] = '\0'; // Null-terminate the string
    fclose(file);

    // Create and build program
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, &kernel_size, &err);
    free(kernel_source);

    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to create program\n");
        exit(1);
    }

    // err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    err = clBuildProgram(program, 1, &device, "-I .", NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "Program build failed: %s\n", log);
        free(log);
        exit(1);
    }

    // Create kernel
    kernel = clCreateKernel(program, "md5_opencl_kernel", &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to create kernel\n");
        exit(1);
    }
}


// Main search function
static void deti_coins_opencl_search(u32_t random_words) {
    printf("Inside deti_coins_opencl_search with random_words = %u\n", random_words);  // Debug print

    const u32_t block_size = 128;
    const u32_t size = 1024u;
    cl_int err;

    // Initialize data
    u32_t n_attempts = 0, n_coins = 0;
    u32_t idx, max_idx, custom_word1, custom_word2;
    custom_word1 = custom_word2 = 0x20202020;
    max_idx = 1u;

    // Initialize OpenCL
    initialize_opencl("md5_opencl_kernel.cl");

    // Create device buffer
    device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, 
                                 size * sizeof(u32_t), NULL, &err);
    
    u32_t *host_data = (u32_t *)malloc(size * sizeof(u32_t));

    while (stop_request == 0) {  // Check stop_request flag
        host_data[0] = 1u;

        // Copy data to device
        err = clEnqueueWriteBuffer(queue, device_data, CL_TRUE, 0,
                                   size * sizeof(u32_t), host_data, 0, NULL, NULL);

        // Set kernel arguments
        clSetKernelArg(kernel, 0, sizeof(u32_t), &custom_word1);
        clSetKernelArg(kernel, 1, sizeof(u32_t), &custom_word2);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &device_data);

        // Launch kernel
        size_t global_work_size = 1 << 20;
        size_t local_work_size = block_size;
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
                                      &global_work_size, &local_work_size,
                                      0, NULL, NULL);

        // Read results
        err = clEnqueueReadBuffer(queue, device_data, CL_TRUE, 0,
                                  size * sizeof(u32_t), host_data, 0, NULL, NULL);

        // Process results
        if (host_data[0] > max_idx)
            max_idx = host_data[0];

        for (idx = 1u; idx < host_data[0] && idx <= size - 13u; idx += 13) {
            if (idx <= size - 13) {
                save_deti_coin(&host_data[idx]);  // Ensure save_deti_coin is declared and defined
                n_coins++;
            } else {
                fprintf(stderr, "deti_coins_opencl_search: wasted DETI coin\n");
            }
        }

        // Update words
        if (custom_word1 != 0x7E7E7E7Eu)
            custom_word1 = next_value_to_try_ascii(custom_word1);
        else {
            custom_word1 = 0x20202020u;
            custom_word2 = next_value_to_try_ascii(custom_word2);
        }

        n_attempts += (1 << 20) * 64u;
    }

    // Cleanup OpenCL resources
    free(host_data);
    clReleaseMemObject(device_data);

    STORE_DETI_COINS();  // Ensure STORE_DETI_COINS is defined
    printf("deti_coins_opencl_search: %u DETI coin%s found in %u attempt%s (expected %.2f coins)\n",
           n_coins, (n_coins == 1ul) ? "" : "s",
           n_attempts, (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ull << 32));

    // Cleanup OpenCL
    cleanup_opencl();  // Call cleanup
}

// Cleanup function definition
static void cleanup_opencl(void) {
    if (kernel) clReleaseKernel(kernel);
    if (program) clReleaseProgram(program);
    if (queue) clReleaseCommandQueue(queue);
    if (context) clReleaseContext(context);
}

#endif // DETI_COINS_OPENCL_H
