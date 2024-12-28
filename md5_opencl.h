#ifndef MD5_OPENCL_H
#define MD5_OPENCL_H

#include <CL/cl.h>

// OpenCL context variables (should be initialized elsewhere)
extern cl_context context;
extern cl_command_queue queue;
extern cl_kernel kernel;
extern cl_mem device_data;
extern cl_mem device_hash;

static void md5_opencl(u32_t *data, u32_t *hash, u32_t n_messages)
{
    cl_int err;
    u32_t n, lane, idx, *ptr;

    if(n_messages % 128u != 0u)
    {
        fprintf(stderr, "initialize_opencl: n_messages is not a multiple of 128\n");
        exit(1);
    }

    // Allocate host buffers if not already allocated
    u32_t *host_data = (u32_t *)malloc(n_messages * 13u * sizeof(u32_t));
    u32_t *host_hash = (u32_t *)malloc(n_messages * 4u * sizeof(u32_t));

    // Data interleaving (same as CUDA version)
    ptr = host_data;
    for(n = 0u; n < n_messages; n += 32u)
    {
        for(lane = 0u; lane < 32u; lane++)                     
            for(idx = 0u; idx < 13u; idx++)                      
                ptr[32u * idx + lane] = data[13u * lane + idx];  
        data = &data[13u * 32u];
        ptr = &ptr[13u * 32u];
    }

    // Create device buffers
    if (device_data == NULL) {
        device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, 
            n_messages * 13u * sizeof(u32_t), NULL, &err);
        if (err != CL_SUCCESS) {
            fprintf(stderr, "Failed to create device_data buffer\n");
            exit(1);
        }
    }

    if (device_hash == NULL) {
        device_hash = clCreateBuffer(context, CL_MEM_READ_WRITE, 
            n_messages * 4u * sizeof(u32_t), NULL, &err);
        if (err != CL_SUCCESS) {
            fprintf(stderr, "Failed to create device_hash buffer\n");
            exit(1);
        }
    }

    // Transfer data to device
    err = clEnqueueWriteBuffer(queue, device_data, CL_TRUE, 0,
        n_messages * 13u * sizeof(u32_t), host_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to write to device_data buffer\n");
        exit(1);
    }

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_data);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_hash);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to set kernel arguments\n");
        exit(1);
    }

    // Launch kernel
    size_t global_work_size = n_messages;
    size_t local_work_size = 128u;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
        &global_work_size, &local_work_size, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to launch kernel\n");
        exit(1);
    }

    // Read results back
    err = clEnqueueReadBuffer(queue, device_hash, CL_TRUE, 0,
        n_messages * 4u * sizeof(u32_t), host_hash, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Failed to read from device_hash buffer\n");
        exit(1);
    }

    // Ensure all commands are complete
    clFinish(queue);

    // Deinterleave results (same as CUDA version)
    ptr = host_hash;
    for(n = 0u; n < n_messages; n += 32u)
    {
        for(lane = 0u; lane < 32u; lane++)                    
            for(idx = 0u; idx < 4u; idx++)                      
                hash[4u * lane + idx] = ptr[32u * idx + lane];  
        ptr = &ptr[4u * 32u];
        hash = &hash[4u * 32u];
    }

    // Cleanup
    free(host_data);
    free(host_hash);
}

static void test_md5_opencl(void)
{
    u32_t n, idx, hash[4];

    // Initialize OpenCL (should be done elsewhere)
    // initialize_opencl(/*appropriate parameters*/);

    time_measurement();
    md5_opencl(host_md5_test_data, host_md5_test_hash, N_MESSAGES);
    time_measurement();

    // Test results
    for(n = 0u; n < N_MESSAGES; n++)
    {
        md5_cpu(&host_md5_test_data[13u * n], &hash[0u]);
        for(idx = 0u; idx < 4u; idx++)
            if(host_md5_test_hash[4u * n + idx] != hash[idx])
            {
                fprintf(stderr, "test_md5_opencl: MD5 hash error for message %u\n", n);
                exit(1);
            }
    }

    printf("time per md5 hash (OpenCL): %7.3fns %7.3fns\n",
        cpu_time_delta_ns() / (double)N_MESSAGES,
        wall_time_delta_ns() / (double)N_MESSAGES);

    // Cleanup OpenCL resources (should be done elsewhere)
    // cleanup_opencl();
}

#endif // MD5_OPENCL_H