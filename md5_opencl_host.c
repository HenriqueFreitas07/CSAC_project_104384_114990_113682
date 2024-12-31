#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE (0x100000)
#define N_MESSAGES 128

void check_error(cl_int ret, const char *message) {
    if (ret != CL_SUCCESS) {
        printf("%s failed with error code %d\n", message, ret);
        exit(1);
    }
}
// criar contexto
// criar queue
// carregar kernel
// criar buffers
// executar kernel
// recuperar dados
// liberar recursos
// fim
int main() {
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    check_error(ret, "clGetPlatformIDs");

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    check_error(ret, "clGetDeviceIDs");

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    check_error(ret, "clCreateContext");

    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
    check_error(ret, "clCreateCommandQueue");

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("opencl_md5_kernel.cl", "r");
    if (!fp) {
        printf("Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

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

    cl_kernel kernel = clCreateKernel(program, "opencl_md5_kernel", &ret);
    check_error(ret, "clCreateKernel");

    cl_mem data_storage_device = clCreateBuffer(context, CL_MEM_READ_WRITE, 1024 * sizeof(u32_t), NULL, &ret);
    check_error(ret, "clCreateBuffer for data_storage_device");

    u32_t data_storage[1024] = {0};  
    ret = clEnqueueWriteBuffer(command_queue, data_storage_device, CL_TRUE, 0, sizeof(data_storage), data_storage, 0, NULL, NULL);
    check_error(ret, "clEnqueueWriteBuffer");

    // argumentos para o kernel
    u32_t v1 = 0x12345678; 
    u32_t v2 = 0x87654321;  

    ret = clSetKernelArg(kernel, 0, sizeof(u32_t), &v1);
    check_error(ret, "clSetKernelArg v1");

    ret = clSetKernelArg(kernel, 1, sizeof(u32_t), &v2);
    check_error(ret, "clSetKernelArg v2");

    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &data_storage_device);
    check_error(ret, "clSetKernelArg data_storage_device");

    size_t global_item_size = 128;  
    size_t local_item_size = 32;   

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    check_error(ret, "clEnqueueNDRangeKernel");

    ret = clEnqueueReadBuffer(command_queue, data_storage_device, CL_TRUE, 0, sizeof(data_storage), data_storage, 0, NULL, NULL);
    check_error(ret, "clEnqueueReadBuffer");

    for (int i = 0; i < 1024; i++) {
        printf("%02x ", data_storage[i]);
    }

    ret = clFlush(command_queue);
    check_error(ret, "clFlush");

    ret = clFinish(command_queue);
    check_error(ret, "clFinish");

    ret = clReleaseKernel(kernel);
    check_error(ret, "clReleaseKernel");

    ret = clReleaseProgram(program);
    check_error(ret, "clReleaseProgram");

    ret = clReleaseMemObject(data_storage_device);
    check_error(ret, "clReleaseMemObject");

    ret = clReleaseCommandQueue(command_queue);
    check_error(ret, "clReleaseCommandQueue");

    ret = clReleaseContext(context);
    check_error(ret, "clReleaseContext");

    free(source_str);

    return 0;
}
