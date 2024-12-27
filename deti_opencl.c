#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "deti_opencl.h"

// OpenCL mining function
void deti_opencl_opencl(unsigned int seconds)
{
    cl_int ret;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_kernel kernel;
    cl_program program;

    // Get OpenCL platform and device
    ret = clGetPlatformIDs(1, &platform, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to get platform!\n");
        return;
    }
    ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to get device!\n");
        return;
    }

    // Create OpenCL context and queue
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &ret);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to create context!\n");
        return;
    }
    queue = clCreateCommandQueue(context, device, 0, &ret);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to create command queue!\n");
        return;
    }

    // Load and compile OpenCL kernel
    const char *kernel_source = load_kernel_source("deti_opencl_kernel.cl");
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &ret);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to create program!\n");
        return;
    }
    ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to build program!\n");
        return;
    }
    kernel = clCreateKernel(program, "deti_opencl_kernel", &ret);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to create kernel!\n");
        return;
    }

    // Set kernel arguments, memory buffers, and enqueue execution
    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to create buffer!\n");
        return;
    }

    ret = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &seconds, NULL, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        printf("Error: Failed to enqueue kernel!\n");
        return;
    }

    // Wait for kernel execution to finish
    clFinish(queue);

    // Clean up OpenCL resources
    clReleaseMemObject(buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    printf("Mining completed.\n");
}
