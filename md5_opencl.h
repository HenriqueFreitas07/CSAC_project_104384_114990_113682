#if USE_OPENCL > 0
#define MD5_OPENCL

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "opencl_driver_api_utilities.h"  

typedef uint32_t u32_t; // Define type alias for OpenCL compatibility

// Declare OpenCL Resources as extern, defined in a corresponding .c file
extern cl_command_queue command_queue;
extern cl_mem device_data;
extern cl_mem device_hash;
extern cl_kernel kernel;  // Add the kernel declaration

extern u32_t *host_md5_test_data; // Declare it if defined in another source
extern u32_t *host_md5_test_hash; // Declare it if defined in another source

// Function Prototypes
void initialize_opencl(int device_id, const char *kernel_file, const char *kernel_name, size_t data_size, size_t hash_size);
void terminate_opencl(void);
static void time_measurement(void);
double cpu_time_delta_ns(void);
double wall_time_delta_ns(void);
void md5_cpu(const u32_t *data, u32_t *hash);

// Declare host data for MD5 test (ensure these are defined elsewhere in your project)
extern u32_t *host_md5_test_data;
extern u32_t *host_md5_test_hash;

// Main md5_opencl Function
static void md5_opencl(u32_t *data, u32_t *hash, u32_t n_messages)
{
    u32_t n, lane, idx, *ptr;
    size_t global_work_size = n_messages / 128u;
    size_t local_work_size = 128u;

    if (n_messages % 128u != 0u)
    {
        fprintf(stderr, "initialize_opencl: n_messages is not a multiple of 128\n");
        exit(1);
    }

    // Allocate host_data if not already allocated
    if (!host_data) host_data = (u32_t *)malloc(n_messages * 13 * sizeof(u32_t));
    if (!host_hash) host_hash = (u32_t *)malloc(n_messages * 4 * sizeof(u32_t));

    // Data -> host_data with 32-way interleaving
    ptr = host_data;
    for (n = 0u; n < n_messages; n += 32u)
    {
        for (lane = 0u; lane < 32u; lane++)                     // For each message number
            for (idx = 0u; idx < 13u; idx++)                    // For each message word
                ptr[32u * idx + lane] = data[13u * lane + idx]; // Interleave
        data = &data[13u * 32u];
        ptr = &ptr[13u * 32u];
    }

    // Transfer data to device
    CL_CALL(clEnqueueWriteBuffer, (command_queue, device_data, CL_TRUE, 0,
                                   n_messages * 13 * sizeof(u32_t), host_data, 0, NULL, NULL));

    // Launch kernel
    CL_CALL(clSetKernelArg, (kernel, 0, sizeof(cl_mem), &device_data));  // Use kernel variable
    CL_CALL(clSetKernelArg, (kernel, 1, sizeof(cl_mem), &device_hash));  // Use kernel variable
    CL_CALL(clEnqueueNDRangeKernel, (command_queue, kernel, 1, NULL,
                                     &global_work_size, &local_work_size, 0, NULL, NULL));
    CL_CALL(clFinish, (command_queue));

    // Transfer hash back to host
    CL_CALL(clEnqueueReadBuffer, (command_queue, device_hash, CL_TRUE, 0,
                                  n_messages * 4 * sizeof(u32_t), host_hash, 0, NULL, NULL));

    // host_hash -> hash with 32-way interleaving
    ptr = host_hash;
    for (n = 0u; n < n_messages; n += 32u)
    {
        for (lane = 0u; lane < 32u; lane++)                     // For each message number
            for (idx = 0u; idx < 4u; idx++)                     // For each message word
                hash[4u * lane + idx] = ptr[32u * idx + lane];  // Deinterleave
        ptr = &ptr[4u * 32u];
        hash = &hash[4u * 32u];
    }

    // Free allocated host buffers
    free(host_data);
    free(host_hash);
    host_data = NULL;
    host_hash = NULL;
}

// Correctness test of md5_opencl()
static void test_md5_opencl(void)
{
    u32_t n, idx, hash[4];

    // Ensure N_MESSAGES is defined before using
    #ifndef N_MESSAGES
    #define N_MESSAGES 1000 // Default value if not defined elsewhere
    #endif

    // Declare host test data and hashes
    // Ensure these are declared and defined elsewhere in your codebase
    extern u32_t *host_md5_test_data; // Declare it if defined in another source
    extern u32_t *host_md5_test_hash; // Declare it if defined in another source

    // Initialize OpenCL
    initialize_opencl(0, "md5_opencl_kernel.cl", "opencl_md5_kernel", 13u * N_MESSAGES, 4u * N_MESSAGES);

    // Time measurement before and after OpenCL execution
    time_measurement();
    md5_opencl(host_md5_test_data, host_md5_test_hash, N_MESSAGES);
    time_measurement();

    // Terminate OpenCL
    terminate_opencl();

    // Verify OpenCL results with CPU-based MD5 computation
    for (n = 0u; n < N_MESSAGES; n++)
    {
        md5_cpu(&host_md5_test_data[13u * n], &hash[0u]);
        for (idx = 0u; idx < 4u; idx++)
            if (host_md5_test_hash[4u * n + idx] != hash[idx])
            {
                fprintf(stderr, "test_md5_opencl: MD5 hash error for message %u\n", n);
                exit(1);
            }
    }

    // Print time statistics for OpenCL processing
    printf("time per md5 hash (OpenCL): %7.3fns %7.3fns\n",
           cpu_time_delta_ns() / (double)N_MESSAGES, wall_time_delta_ns() / (double)N_MESSAGES);
}

#endif // USE_OPENCL > 0
