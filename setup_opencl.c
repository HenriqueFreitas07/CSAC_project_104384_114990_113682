#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE (0x100000)

cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel kernel;

cl_platform_id platform;
cl_device_id device;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;

// Initialize OpenCL
void initialize_opencl(const char *kernel_file, const char *kernel_name) {
    // Get platform and device
    clGetPlatformIDs(1, &platform, &ret_num_platforms);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &ret_num_devices);

    // Create context and command queue
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);

    // Read OpenCL kernel from file
    FILE *fp = fopen(kernel_file, "r");
    if (!fp) {
        perror("Failed to open kernel file");
        exit(1);
    }

    char *source_str = (char *)malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Create program from source
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, NULL);
    free(source_str);

    // Build program
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Create kernel
    kernel = clCreateKernel(program, kernel_name, NULL);
}

// Cleanup OpenCL resources
void cleanup_opencl() {
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
