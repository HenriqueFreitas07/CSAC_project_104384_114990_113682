#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE (0x100000)

void check_error(cl_int ret, const char *message) {
    if (ret != CL_SUCCESS) {
        printf("%s failed with error code %d\n", message, ret);
        exit(1);
    }
}

int main() {
    // Step 1: Get Platform and Device Info
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    check_error(ret, "clGetPlatformIDs");

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    check_error(ret, "clGetDeviceIDs");

    // Step 2: Print Device Information
    char device_name[1024];
    ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
    check_error(ret, "clGetDeviceInfo (CL_DEVICE_NAME)");

    char device_version[1024];
    ret = clGetDeviceInfo(device_id, CL_DEVICE_VERSION, sizeof(device_version), device_version, NULL);
    check_error(ret, "clGetDeviceInfo (CL_DEVICE_VERSION)");

    char device_opencl_version[1024];
    ret = clGetDeviceInfo(device_id, CL_DEVICE_OPENCL_C_VERSION, sizeof(device_opencl_version), device_opencl_version, NULL);
    check_error(ret, "clGetDeviceInfo (CL_DEVICE_OPENCL_C_VERSION)");

    printf("Device Name: %s\n", device_name);
    printf("Device Version: %s\n", device_version);
    printf("Device OpenCL C Version: %s\n", device_opencl_version);

    // Step 3: Create an OpenCL Context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    check_error(ret, "clCreateContext");

    // Step 4: Create a Command Queue
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
    check_error(ret, "clCreateCommandQueue");

    // Step 5: Load the Kernel Source Code
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("opencl_kernel.cl", "r");
    if (!fp) {
        printf("Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Step 6: Create Kernel Program from Source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, &source_size, &ret);
    check_error(ret, "clCreateProgramWithSource");

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("Error in program build: %s\n", buffer);
        exit(1);
    }

    // Step 7: Create the OpenCL Kernel
    cl_kernel kernel = clCreateKernel(program, "hello_world", &ret);
    check_error(ret, "clCreateKernel");

    // Step 8: Allocate Memory for Output
    char output[6] = {0};
    cl_mem output_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 6 * sizeof(char), NULL, &ret);
    check_error(ret, "clCreateBuffer");

    // Step 9: Set Kernel Arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &output_mem);
    check_error(ret, "clSetKernelArg");

    // Step 10: Execute the Kernel
    size_t global_item_size = 5; // 5 characters: 'H', 'I', 'J', 'K', 'L'
    size_t local_item_size = 1; 
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    check_error(ret, "clEnqueueNDRangeKernel");

    // Step 11: Read the Output Data
    ret = clEnqueueReadBuffer(command_queue, output_mem, CL_TRUE, 0, 6 * sizeof(char), output, 0, NULL, NULL);
    check_error(ret, "clEnqueueReadBuffer");

    // Print the Output
    printf("Output from OpenCL Kernel: %s\n", output);

    // Cleanup
    ret = clFlush(command_queue);
    check_error(ret, "clFlush");

    ret = clFinish(command_queue);
    check_error(ret, "clFinish");

    ret = clReleaseKernel(kernel);
    check_error(ret, "clReleaseKernel");

    ret = clReleaseProgram(program);
    check_error(ret, "clReleaseProgram");

    ret = clReleaseMemObject(output_mem);
    check_error(ret, "clReleaseMemObject");

    ret = clReleaseCommandQueue(command_queue);
    check_error(ret, "clReleaseCommandQueue");

    ret = clReleaseContext(context);
    check_error(ret, "clReleaseContext");

    free(source_str);

    return 0;
}
