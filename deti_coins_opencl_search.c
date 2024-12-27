// // // deti_coins_opencl_search.c
// // #include "deti_coins_opencl_search.h"
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <CL/cl.h>

// // // OpenCL variables
// // cl_platform_id platform;
// // cl_device_id device;
// // cl_context context;
// // cl_command_queue queue;
// // cl_program program;
// // cl_kernel kernel;

// // void initialize_opencl(const char* kernel_file, const char* kernel_function) {
// //     cl_int err;
    
// //     // Get platform and device
// //     err = clGetPlatformIDs(1, &platform, NULL);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to get platform.\n");
// //         exit(1);
// //     }
    
// //     err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to get device.\n");
// //         exit(1);
// //     }
    
// //     // Create OpenCL context and command queue
// //     context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to create context.\n");
// //         exit(1);
// //     }
    
// //     queue = clCreateCommandQueue(context, device, 0, &err);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to create command queue.\n");
// //         exit(1);
// //     }
    
// //     // Read OpenCL kernel source code
// //     FILE* kernel_file_ptr = fopen(kernel_file, "r");
// //     if (!kernel_file_ptr) {
// //         printf("Error: Unable to open kernel file.\n");
// //         exit(1);
// //     }
    
// //     fseek(kernel_file_ptr, 0, SEEK_END);
// //     size_t kernel_size = ftell(kernel_file_ptr);
// //     fseek(kernel_file_ptr, 0, SEEK_SET);
// //     char* kernel_source = (char*)malloc(kernel_size + 1);
// //     fread(kernel_source, 1, kernel_size, kernel_file_ptr);
// //     fclose(kernel_file_ptr);
    
// //     // Create and build OpenCL program
// //     program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_size, &err);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to create program.\n");
// //         exit(1);
// //     }
    
// //     err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to build program.\n");
// //         exit(1);
// //     }
    
// //     // Create OpenCL kernel
// //     kernel = clCreateKernel(program, kernel_function, &err);
// //     if (err != CL_SUCCESS) {
// //         printf("Error: Unable to create kernel.\n");
// //         exit(1);
// //     }
// // }

// // void deti_coins_opencl_search(void) {
// //     // Here you would set up buffers, arguments, and call the OpenCL kernel.
// //     // For example, creating buffer objects for input/output data, and executing the kernel.
// //     printf("Running OpenCL DETI coin search...\n");
// //     // Example: clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
// // }

// // void terminate_opencl(void) {
// //     // Clean up OpenCL resources
// //     clReleaseKernel(kernel);
// //     clReleaseProgram(program);
// //     clReleaseCommandQueue(queue);
// //     clReleaseContext(context);
// //     printf("OpenCL resources released.\n");
// // }
// #include <CL/cl.h>
// #include <stdio.h>
// #include <stdlib.h>

// #define N_LANES 4

// // Function to initialize OpenCL, load kernels, and create buffers
// void initialize_opencl(const char *kernel_file, const char *kernel_name) {
//     cl_platform_id platform;
//     clGetPlatformIDs(1, &platform, NULL);

//     cl_device_id device;
//     clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

//     cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
//     cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

//     // Load the OpenCL kernel from file
//     FILE *kernel_file_ptr = fopen(kernel_file, "r");
//     fseek(kernel_file_ptr, 0, SEEK_END);
//     size_t kernel_size = ftell(kernel_file_ptr);
//     rewind(kernel_file_ptr);
//     char *kernel_source = (char *)malloc(kernel_size);
//     fread(kernel_source, 1, kernel_size, kernel_file_ptr);
//     fclose(kernel_file_ptr);

//     cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, &kernel_size, NULL);
//     clBuildProgram(program, 1, &device, NULL, NULL, NULL);

//     cl_kernel kernel = clCreateKernel(program, kernel_name, NULL);

//     // Create buffers for coins and hashes
//     cl_mem coins_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(uint) * 13 * N_LANES, NULL, NULL);
//     cl_mem hashes_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(uint) * 4 * N_LANES, NULL, NULL);

//     // Create your coin data (same structure as before)
//     uint coins[13 * N_LANES];  // Fill this array with the coins data as before

//     // Copy the coin data to the buffer
//     clEnqueueWriteBuffer(queue, coins_buffer, CL_TRUE, 0, sizeof(uint) * 13 * N_LANES, coins, 0, NULL, NULL);

//     // Set kernel arguments
//     clSetKernelArg(kernel, 0, sizeof(cl_mem), &coins_buffer);
//     clSetKernelArg(kernel, 1, sizeof(cl_mem), &hashes_buffer);
//     clSetKernelArg(kernel, 2, sizeof(uint), &N_LANES);

//     // Execute the kernel
//     size_t global_size = N_LANES;
//     clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

//     // Read the results back from the device
//     uint hashes[4 * N_LANES];
//     clEnqueueReadBuffer(queue, hashes_buffer, CL_TRUE, 0, sizeof(uint) * 4 * N_LANES, hashes, 0, NULL, NULL);

//     // Process the result and check for DETI coins (check if any hash is 0)
//     for (int lane = 0; lane < N_LANES; lane++) {
//         if (hashes[3 * N_LANES + lane] == 0) {
//             // Save the coin if it is a DETI coin (hash check is valid)
//             // You can call save_deti_coin() or another function as needed
//             printf("FOUND ONE: %52.52s\n", (char *)coins + lane * 13);
//         }
//     }

//     // Clean up OpenCL resources
//     clReleaseMemObject(coins_buffer);
//     clReleaseMemObject(hashes_buffer);
//     clReleaseKernel(kernel);
//     clReleaseProgram(program);
//     clReleaseCommandQueue(queue);
//     clReleaseContext(context);
// }

// void deti_coins_opencl_search() {
//     // Initialize OpenCL
//     initialize_opencl("deti_coins_opencl_kernel.cl", "deti_coins_opencl_kernel");
// }
// deti_coins_opencl_search.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>

// Include MD5 hashing functions or any other utility functions you need
#include "md5.h"

// OpenCL kernel source code (this could be in a separate .cl file, but we'll define it here for simplicity)
const char* opencl_kernel_source = R"(
__kernel void md5_search(__global const unsigned char *input, __global unsigned char *output, const unsigned int num_words) {
    int id = get_global_id(0);
    
    if (id < num_words) {
        // Perform MD5 hashing on input data (this is just an example, replace with actual logic)
        unsigned char data[64];
        for (int i = 0; i < 64; i++) {
            data[i] = input[id * 64 + i];
        }
        md5_hash(data, 64, output + id * 16);  // Assuming md5_hash() is a function that performs MD5 on the data
    }
}
)";

// OpenCL context, queue, and program variables
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel kernel;
cl_device_id device;
cl_platform_id platform;
cl_int ret;

#define MAX_SOURCE_SIZE (0x10000)

void initialize_opencl() {
    // Get platform and device information
    ret = clGetPlatformIDs(1, &platform, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to get platform ID\n");
        exit(1);
    }

    ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to get device ID\n");
        exit(1);
    }

    // Create an OpenCL context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create context\n");
        exit(1);
    }

    // Create a command queue
    queue = clCreateCommandQueue(context, device, 0, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create command queue\n");
        exit(1);
    }
}

void load_opencl_kernel() {
    // Load the kernel source code (you can also load it from an external file)
    const char* kernel_source = opencl_kernel_source;
    size_t source_size = strlen(kernel_source);

    // Create the OpenCL program from the source code
    program = clCreateProgramWithSource(context, 1, &kernel_source, &source_size, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create program\n");
        exit(1);
    }

    // Build the program
    ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to build program\n");
        exit(1);
    }

    // Create the OpenCL kernel
    kernel = clCreateKernel(program, "md5_search", &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create kernel\n");
        exit(1);
    }
}

void deti_coins_opencl_search(unsigned int num_random_words) {
    // Prepare input and output data (replace with actual data for your use case)
    unsigned char *input_data = (unsigned char*)malloc(num_random_words * 64 * sizeof(unsigned char));
    unsigned char *output_data = (unsigned char*)malloc(num_random_words * 16 * sizeof(unsigned char));  // MD5 produces 16-byte output
    
    // Create OpenCL buffers
    cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, num_random_words * 64 * sizeof(unsigned char), NULL, &ret);
    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, num_random_words * 16 * sizeof(unsigned char), NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create buffers\n");
        exit(1);
    }

    // Write data to input buffer
    ret = clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, 0, num_random_words * 64 * sizeof(unsigned char), input_data, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to write to input buffer\n");
        exit(1);
    }

    // Set kernel arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
    ret |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &num_random_words);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to set kernel arguments\n");
        exit(1);
    }

    // Execute the kernel
    size_t global_work_size = num_random_words;  // Number of work items (one per word)
    size_t local_work_size = 64;  // Local work size (this could depend on your hardware, for now we use 64)

    ret = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to enqueue kernel\n");
        exit(1);
    }

    // Read the results from the output buffer
    ret = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, num_random_words * 16 * sizeof(unsigned char), output_data, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to read output buffer\n");
        exit(1);
    }

    // Process results (e.g., print the hashes or count valid DETI coins)
    for (unsigned int i = 0; i < num_random_words; i++) {
        printf("MD5 hash of word %u: ", i);
        for (int j = 0; j < 16; j++) {
            printf("%02x", output_data[i * 16 + j]);
        }
        printf("\n");
    }

    // Clean up OpenCL resources
    clReleaseMemObject(input_buffer);
    clReleaseMemObject(output_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // Free allocated memory
    free(input_data);
    free(output_data);
}

int main(int argc, char **argv) {
    unsigned int num_random_words = 1000;  // Number of random words to search for (example)
    
    // Initialize OpenCL
    initialize_opencl();

    // Load the kernel
    load_opencl_kernel();

    // Run the search for DETI coins using OpenCL
    deti_coins_opencl_search(num_random_words);

    return 0;
}
