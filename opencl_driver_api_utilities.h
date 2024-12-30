#ifndef OPENCL_DRIVER_API_UTILITIES
#define OPENCL_DRIVER_API_UTILITIES

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

//
// CL_CALL --- macro to check OpenCL API return values
//

#define CL_CALL(f_name, args)                                                           \
    do {                                                                                \
        cl_int e = f_name args;                                                         \
        if (e != CL_SUCCESS) {                                                          \
            fprintf(stderr, "%s() returned %s (file %s, line %d)\n",                     \
                    #f_name, cl_error_string(e), __FILE__, __LINE__);                   \
            exit(1);                                                                    \
        }                                                                               \
    } while (0)

//
// cl_error_string --- return OpenCL error string
//

static const char *cl_error_string(cl_int err) {
    switch (err) {
        case CL_SUCCESS: return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE: return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE: return "CL_MAP_FAILURE";
        case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
        case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
        default: return "UNKNOWN ERROR";
    }
}

//
// OpenCL global variables
//

static cl_platform_id platform_id;
static cl_device_id device_id;
static cl_context context;
static cl_command_queue command_queue;
static cl_program program;
static cl_kernel kernel;

static cl_mem device_data;
static cl_mem device_hash;
static uint32_t *host_data;
static uint32_t *host_hash;

//
// Initialize OpenCL
//

static void initialize_opencl(int device_number, const char *program_source, const char *kernel_name, size_t data_size, size_t hash_size) {
    cl_int err;
    cl_uint num_platforms;
    cl_uint num_devices;

    // Get Platform and Device Info
    CL_CALL(clGetPlatformIDs, (1, &platform_id, &num_platforms));
    CL_CALL(clGetDeviceIDs, (platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices));

    // Create Context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error creating OpenCL context\n");
        exit(1);
    }

    // Create Command Queue (Updated for deprecation warning)
    command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    CL_CALL(clCreateCommandQueueWithProperties, (context, device_id, 0, &err));

    // Build Program
    program = clCreateProgramWithSource(context, 1, &program_source, NULL, &err);
    CL_CALL(clCreateProgramWithSource, (context, 1, &program_source, NULL, &err));
    CL_CALL(clBuildProgram, (program, 1, &device_id, NULL, NULL, NULL));

    // Create Kernel
    kernel = clCreateKernel(program, kernel_name, &err);
    CL_CALL(clCreateKernel, (program, kernel_name, &err));

    // Allocate Host Memory
    if (data_size > 0) {
        host_data = (uint32_t *)malloc(data_size * sizeof(uint32_t));
    }
    if (hash_size > 0) {
        host_hash = (uint32_t *)malloc(hash_size * sizeof(uint32_t));
    }

    // Allocate Device Memory
    if (data_size > 0) {
        device_data = clCreateBuffer(context, CL_MEM_READ_WRITE, data_size * sizeof(uint32_t), NULL, &err);
        CL_CALL(clCreateBuffer, (context, CL_MEM_READ_WRITE, data_size * sizeof(uint32_t), NULL, &err));
    }
    if (hash_size > 0) {
        device_hash = clCreateBuffer(context, CL_MEM_READ_WRITE, hash_size * sizeof(uint32_t), NULL, &err);
        CL_CALL(clCreateBuffer, (context, CL_MEM_READ_WRITE, hash_size * sizeof(uint32_t), NULL, &err));
    }
}


//
// Terminate OpenCL
//

static void terminate_opencl(void) {
    if (host_data != NULL) free(host_data);
    if (host_hash != NULL) free(host_hash);

    if (device_data != NULL) clReleaseMemObject(device_data);
    if (device_hash != NULL) clReleaseMemObject(device_hash);

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}

#endif // OPENCL_DRIVER_API_UTILITIES
