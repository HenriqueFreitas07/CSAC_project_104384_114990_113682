#include <CL/cl.h>

#define DETI_COINS_OPENCL_KERNEL_SEARCH

static void deti_coins_opencl_kernel_search(void)
{
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_mem buffer_data;
    cl_kernel kernel;
    size_t global_work_size, local_work_size;

    // Initialize OpenCL platform, device, context, and queue
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    // Create buffer for data
    buffer_data = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(u32_t) * n_coins * 64, NULL, &err);

    // Load and compile the OpenCL kernel
    kernel = clCreateKernel(program, "deti_kernel", &err);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_data);

    // Launch kernel
    global_work_size = n_coins / 128;
    local_work_size = 128;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

    // Clean up
    clReleaseMemObject(buffer_data);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
