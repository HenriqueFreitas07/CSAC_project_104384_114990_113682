#ifndef DETI_COINS_OPENCL_SEARCH
#define DETI_COINS_OPENCL_SEARCH
#include <time.h>
#include <CL/cl.h>
// em opencl nao preciso dos dois ficheiros host e kernel

long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

u32_t next_value_to_try_ascii(u32_t v){
    v++;                                                
    if ( (v & 0xFF) == 0x7F )                           
    {                                                   
        v += 0xA1; 
        if (((v >> 8) & 0xFF ) == 0x7F)                 
        {                                               
            v += 0xA1 << 8;                             
            if (((v >> 16) & 0xFF ) == 0x7F)            
            {                                           
                v += 0xA1 << 16;                        
                if (((v >> 24) & 0xFF ) == 0x7F)        
                    v += 0xA1 << 24;                    
            }                                           
        }                                               
    }                                                   
    return v; 
}

void deti_coins_opencl_search(u32_t random_words) {
    u32_t block_size = 128;
    u32_t size = 1024u;
    u64_t n_attempts = 0, n_coins = 0, kernel_calls = 0, sum_times_ns = 0;
    u32_t idx, max_idx, custom_word1, custom_word2;
    custom_word1 = custom_word2 = 0x20202020;

    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem device_data;
    cl_mem host_data_buffer;
    
    err = clGetPlatformIDs(1, &platform, NULL);
    assert(err == CL_SUCCESS);
    
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    assert(err == CL_SUCCESS);
    
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    assert(err == CL_SUCCESS);
    
    queue = clCreateCommandQueue(context, device, 0, &err);
    assert(err == CL_SUCCESS);

    const char *kernel_source = load_kernel_source("md5_opencl_kernel.cl");
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    assert(err == CL_SUCCESS);
    
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    assert(err == CL_SUCCESS);
    
    kernel = clCreateKernel(program, "md5_kernel", &err);
    assert(err == CL_SUCCESS);

    device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(u32_t) * size, NULL, &err);
    assert(err == CL_SUCCESS);

    host_data_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(u32_t) * size, NULL, &err);
    assert(err == CL_SUCCESS);

    for (n_attempts = n_coins = 0ul; stop_request == 0; n_attempts += (64ul << 20ul))
    {
        cl_mem host_data = malloc(sizeof(u32_t) * size);
        host_data[0] = 1u;

        err = clEnqueueWriteBuffer(queue, device_data, CL_TRUE, 0, sizeof(u32_t) * size, host_data, 0, NULL, NULL);
        assert(err == CL_SUCCESS);

        err = clSetKernelArg(kernel, 0, sizeof(u32_t), &custom_word1);
        assert(err == CL_SUCCESS);
        
        err = clSetKernelArg(kernel, 1, sizeof(u32_t), &custom_word2);
        assert(err == CL_SUCCESS);
        
        err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &device_data);
        assert(err == CL_SUCCESS);
        
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        size_t global_size = (1 << 20) / block_size;
        size_t local_size = block_size;

        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
        assert(err == CL_SUCCESS);

        err = clFinish(queue);
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        err = clEnqueueReadBuffer(queue, device_data, CL_TRUE, 0, sizeof(u32_t) * size, host_data, 0, NULL, NULL);
        assert(err == CL_SUCCESS);

        if (host_data[0] > max_idx)
            max_idx = host_data[0];

        for (idx = 1u; idx < host_data[0] && idx <= size - 13u; idx += 13)
        {
            if (idx <= size - 13)
            {
                save_deti_coin(&host_data[idx]);
                n_coins++;
            }
            else
            {
                fprintf(stderr, "deti_coins_opencl_search: wasted DETI coin\n");
            }
        }

        if (custom_word1 != 0x7E7E7E7Eu)
        {
            custom_word1 = next_value_to_try_ascii(custom_word1);
        }
        else
        {
            custom_word1 = 0x20202020u;
            custom_word2 = next_value_to_try_ascii(custom_word2);
        }

        long elapsed_time_ns = calculate_elapsed_time(start_time, end_time);
        sum_times_ns += elapsed_time_ns;
        kernel_calls++;
    }

    double avg_time_us = (double)sum_times_ns / (kernel_calls * 1e3);
    printf("Average kernel execution time: %.3f µs\n", avg_time_us);

    STORE_DETI_COINS();
    printf("deti_coins_opencl_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
           n_coins,
           (n_coins == 1ul) ? "" : "s",
           n_attempts,
           (n_attempts == 1ul) ? "" : "s",
           (double)n_attempts / (double)(1ul << 32));

    clReleaseMemObject(device_data);
    clReleaseMemObject(host_data_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

const char* load_kernel_source(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open kernel file");
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = (char*)malloc(length + 1);
    fread(source, 1, length, file);
    fclose(file);
    
    source[length] = '\0';
    return source;
}

#endif // DETI_COINS_OPENCL_KERNEL_SEARCH
