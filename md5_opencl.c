#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define NUM_BLOCKS 16
#define NUM_THREADS_PER_BLOCK 64

// MD5 Constants
uint32_t k[64] = { ... };  // MD5 constants (should be copied from MD5 spec)
uint32_t s[64] = { ... };  // MD5 shifts (should be copied from MD5 spec)

// OpenCL kernel code
const char *kernelSource = "md5_kernel.cl";  // Path to the kernel file

// Function to load OpenCL source code
const char* load_kernel_source(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open kernel file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    
    char *source = (char*)malloc(size + 1);
    fread(source, size, 1, file);
    source[size] = '\0';

    fclose(file);
    return source;
}

// OpenCL setup and execution
void md5_opencl(uint32_t *data, uint32_t *hash) {
    cl_int err;

    // Get OpenCL platforms and devices
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);
    
    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    // Create OpenCL context and queue
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    // Load and compile kernel
    const char* kernel_source = load_kernel_source(kernelSource);
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "md5_kernel", &err);

    // Create buffers
    cl_mem buffer_data = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(uint32_t) * NUM_BLOCKS * 16, NULL, &err);
    cl_mem buffer_hash = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(uint32_t) * NUM_BLOCKS * 4, NULL, &err);

    // Write data to buffer
    clEnqueueWriteBuffer(queue, buffer_data, CL_TRUE, 0, sizeof(uint32_t) * NUM_BLOCKS * 16, data, 0, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_data);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_hash);

    // Execute kernel
    size_t global_size = NUM_BLOCKS * 16;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &NUM_THREADS_PER_BLOCK, 0, NULL, NULL);
    clFinish(queue);

    // Read results back to host
    clEnqueueReadBuffer(queue, buffer_hash, CL_TRUE, 0, sizeof(uint32_t) * NUM_BLOCKS * 4, hash, 0, NULL, NULL);

    // Clean up
    clReleaseMemObject(buffer_data);
    clReleaseMemObject(buffer_hash);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

int main() {
    // Example input data
    uint32_t data[NUM_BLOCKS * 16] = { ... };  // Input data (16 words per block)
    uint32_t hash[NUM_BLOCKS * 4];  // Output hash (4 words per block)

    // Call the OpenCL-based MD5 function
    md5_opencl(data, hash);

    // Output hash result
    for (int i = 0; i < NUM_BLOCKS * 4; i++) {
        printf("%08x ", hash[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }

    return 0;
}


// Create/OpenCL Setup File (md5_opencl.c): This file will contain the host code to manage OpenCL resources like contexts, command queues, and buffers. It will also handle kernel loading and execution.

// Host Code for MD5 OpenCL (md5_opencl.c):