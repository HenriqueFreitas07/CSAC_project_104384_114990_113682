#include "deti_coins_OpenCL_search.h"

// Function to initialize OpenCL (platform, device, context, and queue)
int init_opencl(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *queue) {
    cl_int err;
    
    // Get the platform
    err = clGetPlatformIDs(1, platform, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting OpenCL platform\n");
        return -1;
    }

    // Get the device (here we're using the first available GPU)
    err = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, 1, device, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting OpenCL device\n");
        return -1;
    }

    // Create the context
    *context = clCreateContext(NULL, 1, device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating OpenCL context\n");
        return -1;
    }

    // Create the command queue
    *queue = clCreateCommandQueue(*context, *device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating OpenCL command queue\n");
        return -1;
    }

    return 0;
}

// Function to load and compile the OpenCL kernel
int load_kernel(const char *kernel_file, cl_program *program, cl_kernel *kernel, cl_context context, cl_device_id device) {
    FILE *file = fopen(kernel_file, "r");
    if (!file) {
        printf("Error opening kernel file\n");
        return -1;
    }
    
    // Read the kernel code from the file
    fseek(file, 0, SEEK_END);
    long kernel_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *kernel_code = (char *)malloc(kernel_size + 1);
    fread(kernel_code, 1, kernel_size, file);
    fclose(file);

    kernel_code[kernel_size] = '\0';

    cl_int err;
    
    // Create the OpenCL program
    *program = clCreateProgramWithSource(context, 1, (const char **)&kernel_code, &kernel_size, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating OpenCL program\n");
        return -1;
    }

    // Build the program
    err = clBuildProgram(*program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error building OpenCL program\n");
        return -1;
    }

    // Create the OpenCL kernel
    *kernel = clCreateKernel(*program, "md5_kernel", &err);
    if (err != CL_SUCCESS) {
        printf("Error creating OpenCL kernel\n");
        return -1;
    }

    return 0;
}

// Function to search for a valid hash (coin)
int search_coin_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, uint32_t *data, uint32_t *result_hash) {
    cl_int err;

    cl_mem buffer_data, buffer_result;
    
    // Create buffers
    if (create_opencl_buffers(context, NULL, &buffer_data, &buffer_result, data, result_hash) != 0) {
        return -1;
    }

    size_t global_size = 1;  // For simplicity, we'll search for one hash at a time

    // Execute kernel
    if (execute_kernel(queue, kernel, global_size, buffer_data, buffer_result) != 0) {
        return -1;
    }

    // Read result
    err = clEnqueueReadBuffer(queue, buffer_result, CL_TRUE, 0, sizeof(result_hash), result_hash, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error reading result buffer\n");
        return -1;
    }

    // Check if the hash is valid
    if (check_hash_for_coin(result_hash) == 1) {
        printf("Found valid coin! Hash: ");
        for (int i = 0; i < 4; i++) {
            printf("%08x ", result_hash[i]);
        }
        printf("\n");
        return 1;  // Found a valid coin
    }

    return 0;  // No valid coin found
}

// Function to create OpenCL buffers
int create_opencl_buffers(cl_context context, cl_device_id device, cl_mem *buffer_data, cl_mem *buffer_result, uint32_t *data, uint32_t *result_hash) {
    cl_int err;

    // Create buffers for input data and result hash
    *buffer_data = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(uint32_t) * 16, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating buffer for data\n");
        return -1;
    }

    *buffer_result = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(uint32_t) * 4, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating buffer for result hash\n");
        return -1;
    }

    err = clEnqueueWriteBuffer(context, *buffer_data, CL_TRUE, 0, sizeof(uint32_t) * 16, data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error writing data buffer\n");
        return -1;
    }

    return 0;
}

// Function to execute the OpenCL kernel
int execute_kernel(cl_command_queue queue, cl_kernel kernel, size_t global_size, cl_mem buffer_data, cl_mem buffer_result) {
    cl_int err;

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_data);
    if (err != CL_SUCCESS) {
        printf("Error setting kernel argument for data\n");
        return -1;
    }
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_result);
    if (err != CL_SUCCESS) {
        printf("Error setting kernel argument for result\n");
        return -1;
    }

    // Execute kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error executing kernel\n");
        return -1;
    }

    return 0;
}

// Function to check if the hash is valid
int check_hash_for_coin(uint32_t *hash) {
    // Example: Check if the hash is less than the target difficulty
    if (hash[0] < TARGET_DIFFICULTY) {
        return 1;  // Valid coin found
    }
    return 0;  // Not valid
}
