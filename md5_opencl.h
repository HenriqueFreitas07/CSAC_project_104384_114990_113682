#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define N_MESSAGES 128

// tentei seguir os passos do CUDA e da documentação KHRONOS

static void md5_opencl(u32_t *data, u32_t *hash, u32_t n_messages)
{
    u32_t n, lane, idx, *ptr;

    if (n_messages % 128u != 0u) {
        fprintf(stderr, "initialize_opencl: n_messages is not a multiple of 128\n");
        exit(1);
    }

    ptr = host_data;
    for (n = 0u; n < n_messages; n += 32u) {
        for (lane = 0u; lane < 32u; lane++) { 
            for (idx = 0u; idx < 13u; idx++) {  
                ptr[32u * idx + lane] = data[13u * lane + idx];  
            }
        }
        data = &data[13u * 32u];
        ptr = &ptr[13u * 32u];
    }

    // OpenCL code for kernel setup
    cl_int err;
    cl_mem device_data, device_hash;

    // precisamos de criar um buffer para armazenar os dados na GPU
    // e um buffer para armazenar os hashes na GPU
    device_data = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(u32_t) * 13 * n_messages, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error creating device buffer for data\n");
        exit(1);
    }

    device_hash = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(u32_t) * 4 * n_messages, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error creating device buffer for hash\n");
        exit(1);
    }

    // Copy data to device memory
    err = clEnqueueWriteBuffer(queue, device_data, CL_TRUE, 0, sizeof(u32_t) * 13 * n_messages, host_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error writing data to device\n");
        exit(1);
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_data);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_hash);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error setting kernel arguments\n");
        exit(1);
    }

    // Launch the kernel
    size_t global_size = n_messages / 128u * 128u; 
    size_t local_size = 128u;  
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error launching kernel\n");
        exit(1);
    }

    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error synchronizing OpenCL queue\n");
        exit(1);
    }

    err = clEnqueueReadBuffer(queue, device_hash, CL_TRUE, 0, sizeof(u32_t) * 4 * n_messages, host_hash, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error reading hash from device\n");
        exit(1);
    }

    ptr = host_hash;
    for (n = 0u; n < n_messages; n += 32u) {
        for (lane = 0u; lane < 32u; lane++) {  
            for (idx = 0u; idx < 4u; idx++) {  
                hash[4u * lane + idx] = ptr[32u * idx + lane];  
            }
        }
        ptr = &ptr[4u * 32u];
        hash = &hash[4u * 32u];
    }
}

// OpenCL test function for MD5 computation -> copied
static void test_md5_opencl(void)
{
    u32_t n, idx, hash[4];

    initialize_opencl();  // Function to initialize OpenCL context, queue, etc.
    time_measurement();  // Function to measure execution time

    md5_opencl(host_md5_test_data, host_md5_test_hash, N_MESSAGES);

    time_measurement();  // Measure execution time after the kernel execution
    terminate_opencl();  // Cleanup OpenCL resources

    for (n = 0u; n < N_MESSAGES; n++) {
        md5_cpu(&host_md5_test_data[13u * n], &hash[0u]);
        for (idx = 0u; idx < 4u; idx++) {
            if (host_md5_test_hash[4u * n + idx] != hash[idx]) {
                fprintf(stderr, "test_md5_opencl: MD5 hash error for message %u\n", n);
                exit(1);
            }
        }
    }

    printf("time per MD5 hash (OpenCL): %7.3fns %7.3fns\n", cpu_time_delta_ns() / (double)N_MESSAGES, wall_time_delta_ns() / (double)N_MESSAGES);
}
