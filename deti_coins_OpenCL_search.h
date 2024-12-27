#ifndef DETI_COINS_OPENCL_SEARCH_H
#define DETI_COINS_OPENCL_SEARCH_H

#include <CL/cl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constants (e.g., the target hash difficulty)
#define TARGET_DIFFICULTY 0x0FFFFFFF   // Example difficulty target (adjust as needed)

// Function to initialize OpenCL and get the device
int init_opencl(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *queue);

// Function to load and compile the OpenCL kernel
int load_kernel(const char *kernel_file, cl_program *program, cl_kernel *kernel, cl_context context, cl_device_id device);

// Function to perform the search for the coin (valid hash)
int search_coin_opencl(cl_context context, cl_command_queue queue, cl_kernel kernel, uint32_t *data, uint32_t *result_hash);

// Function to create OpenCL buffers for the data and hash
int create_opencl_buffers(cl_context context, cl_device_id device, cl_mem *buffer_data, cl_mem *buffer_result, uint32_t *data, uint32_t *result_hash);

// Function to set the kernel arguments and execute
int execute_kernel(cl_command_queue queue, cl_kernel kernel, size_t global_size, cl_mem buffer_data, cl_mem buffer_result);

// Function to check if the result hash meets the difficulty (target)
int check_hash_for_coin(uint32_t *hash);

#endif // DETI_COINS_OPENCL_SEARCH_H
