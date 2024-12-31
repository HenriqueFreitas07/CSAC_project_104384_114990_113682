// #ifndef DETI_COINS_OPENCL_KERNEL_SEARCH
// #define DETI_COINS_OPENCL_KERNEL_SEARCH

// #include <CL/cl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>

// typedef unsigned int u32_t;
// typedef unsigned long long u64_t;


// static void deti_coins_opencl_kernel_search(void)
// {
//     cl_int err;
//     u32_t v1 = 0x20202020;
//     u32_t v2 = 0x20202020;
//     u32_t block_size = 128;

//     cl_platform_id platform;
//     clGetPlatformIDs(1, &platform, NULL);

//     cl_device_id device;
//     clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

//     cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
//     if (err != CL_SUCCESS) {
//         printf("Error: Failed to create OpenCL context\n");
//         exit(1);
//     }

//     cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
//     if (err != CL_SUCCESS) {
//         printf("Error: Failed to create OpenCL command queue\n");
//         exit(1);
//     }

//     const char *kernel_file = "md5_opencl_kernel.cl";
//     FILE *kernel_code_file = fopen(kernel_file, "r");
//     if (!kernel_code_file) {
//         printf("Error: Failed to load OpenCL kernel file\n");
//         exit(1);
//     }

//     fseek(kernel_code_file, 0, SEEK_END);
//     size_t kernel_code_size = ftell(kernel_code_file);
//     rewind(kernel_code_file);

//     char *kernel_code = (char *)malloc(kernel_code_size);
//     fread(kernel_code, 1, kernel_code_size, kernel_code_file);
//     fclose(kernel_code_file);

//     cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_code, &kernel_code_size, &err);
//     free(kernel_code);
//     if (err != CL_SUCCESS) {
//         printf("Error: Failed to create OpenCL program\n");
//         exit(1);
//     }

//     err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
//     if (err != CL_SUCCESS) {
//         char build_log[16384];
//         clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
//         printf("Error: Failed to build OpenCL program\nBuild log: %s\n", build_log);
//         exit(1);
//     }

//     cl_kernel kernel = clCreateKernel(program, "opencl_md5_kernel", &err);
//     if (err != CL_SUCCESS) {
//         printf("Error: Failed to create OpenCL kernel\n");
//         exit(1);
//     }

//     // Memory buffers
//     cl_mem buffer_v1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(u32_t), NULL, &err);
//     cl_mem buffer_v2 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(u32_t), NULL, &err);
//     cl_mem buffer_device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(u32_t) * 1024, NULL, &err);

//     // Write data to buffers
//     err = clEnqueueWriteBuffer(queue, buffer_v1, CL_TRUE, 0, sizeof(u32_t), &v1, 0, NULL, NULL);
//     err |= clEnqueueWriteBuffer(queue, buffer_v2, CL_TRUE, 0, sizeof(u32_t), &v2, 0, NULL, NULL);
//     if (err != CL_SUCCESS) {
//         printf("Error: Failed to write to OpenCL buffer\n");
//         exit(1);
//     }

//     u64_t n_attempts = 0, n_coins = 0, kernel_calls = 0, sum_times_ns = 0;
//     u32_t idx, max_idx = 1u;
//     struct timespec start_time, end_time;

//     for (n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (1 << 20) * 64) {
//         next_value_to_try(v1);
//         if (v1 == 0x20202020) {
//             next_value_to_try(v2);
//         }

//         // Set kernel arguments
//         err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_v1);
//         err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_v2);
//         err |= clSetKernelArg(kernel, 2, sizeof(u32_t), &block_size);
//         err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &buffer_device_data);

//         if (err != CL_SUCCESS) {
//             printf("Error: Failed to set OpenCL kernel arguments\n");
//             exit(1);
//         }

//         clock_gettime(CLOCK_MONOTONIC, &start_time);

//         // Launch the OpenCL kernel
//         size_t global_work_size = (1 << 20) / block_size;
//         size_t local_work_size = block_size;

//         err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
//         if (err != CL_SUCCESS) {
//             printf("Error: Failed to enqueue OpenCL kernel\n");
//             exit(1);
//         }

//         // Wait for the kernel to finish
//         clFinish(queue);

//         clock_gettime(CLOCK_MONOTONIC, &end_time);

//         err = clEnqueueReadBuffer(queue, buffer_device_data, CL_TRUE, 0, sizeof(u32_t) * 1024, host_data, 0, NULL, NULL);
//         if (err != CL_SUCCESS) {
//             printf("Error: Failed to read OpenCL buffer\n");
//             exit(1);
//         }

//         if (host_data[0] > max_idx) max_idx = host_data[0];

//         for (idx = 1u; idx < host_data[0] && idx <= 1024 - 13u; idx += 13) {
//             if (idx <= 1024 - 13) {
//                 save_deti_coin(&host_data[idx]);
//                 n_coins++;
//             }
//         }

//         long elapsed_time_ns = calculate_elapsed_time(start_time, end_time);
//         sum_times_ns += elapsed_time_ns;
//         kernel_calls++;
//     }

//     double avg_time_us = (double)sum_times_ns / (kernel_calls * 1e3);
//     printf("Average kernel execution time: %.3f µs\n", avg_time_us);

//     // Terminate OpenCL resources
//     clReleaseMemObject(buffer_v1);
//     clReleaseMemObject(buffer_v2);
//     clReleaseMemObject(buffer_device_data);
//     clReleaseKernel(kernel);
//     clReleaseProgram(program);
//     clReleaseCommandQueue(queue);
//     clReleaseContext(context);

//     STORE_DETI_COINS();
//     printf("deti_coins_opencl_kernel_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
//            n_coins, (n_coins == 1ul) ? "" : "s", n_attempts, (n_attempts == 1ul) ? "" : "s",
//            (double)n_attempts / (double)(1ul << 32));
// }

// #endif // DETI_COINS_OPENCL_KERNEL_SEARCH
