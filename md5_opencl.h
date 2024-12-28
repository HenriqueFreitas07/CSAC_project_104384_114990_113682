#include <CL/cl.h>

#define MD5_OPENCL

static void md5_opencl(u32_t *data, u32_t *hash, u32_t n_messages)
{
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_mem buffer_data, buffer_hash;
    cl_kernel kernel;
    size_t global_work_size, local_work_size;

    // Initialize OpenCL platform, device, context, and queue
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    // Create buffers for data and hash
    buffer_data = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(u32_t) * n_messages * 13, NULL, &err);
    buffer_hash = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(u32_t) * n_messages * 4, NULL, &err);

    // Write data to buffer
    err = clEnqueueWriteBuffer(queue, buffer_data, CL_TRUE, 0, sizeof(u32_t) * n_messages * 13, data, 0, NULL, NULL);

    // Load and compile the OpenCL kernel
    kernel = clCreateKernel(program, "md5_kernel", &err);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_data);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_hash);

    // Launch kernel
    global_work_size = n_messages / 128;
    local_work_size = 128;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

    // Read back the result
    err = clEnqueueReadBuffer(queue, buffer_hash, CL_TRUE, 0, sizeof(u32_t) * n_messages * 4, hash, 0, NULL, NULL);

    // Clean up
    clReleaseMemObject(buffer_data);
    clReleaseMemObject(buffer_hash);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
