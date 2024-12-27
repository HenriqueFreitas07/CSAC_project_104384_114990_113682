#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "setup_opencl.c"  // Include OpenCL setup

volatile int stop_request = 0;

void alarm_signal_handler(int sig) {
    stop_request = 1;  // Stop the loop when the time is up
}


// To decide whether a coin is valid, we check its hash for a specific pattern. For example, if we want to find coins whose hashes end with zeroes, we might check the last few bits:
int count_trailing_zeros(unsigned int hash[4]) {
    int count = 0;
    for (int i = 3; i >= 0; i--) {
        unsigned int value = hash[i];
        while (value && (value & 1) == 0) {
            count++;
            value >>= 1;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <duration in seconds>\n", argv[0]);
        return 1;
    }

    unsigned int duration = atoi(argv[1]);  // Time in seconds
    if (duration <= 0) {
        fprintf(stderr, "Invalid time duration\n");
        return 1;
    }

    // Set up alarm to stop after the specified duration
    signal(SIGALRM, alarm_signal_handler);
    alarm(duration);

    // Initialize OpenCL
    initialize_opencl("md5_kernel.cl", "md5_kernel");

    unsigned int valid_coin_count = 0;
    unsigned int *coins = malloc(sizeof(unsigned int) * 1024);  // Buffer for coins
    unsigned int *hashes = malloc(sizeof(unsigned int) * 1024 * 4);  // Buffer for hashes
    
    cl_mem coins_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int) * 1024, NULL, NULL);
    cl_mem hashes_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * 1024 * 4, NULL, NULL);

    // Start coin generation and search
    while (!stop_request) {
        // Generate coins (this is a placeholder - modify based on actual coin generation)
        for (int i = 0; i < 1024; i++) {
            coins[i] = rand();  // Random coin generation
        }

        // Write coins to OpenCL buffer
        clEnqueueWriteBuffer(queue, coins_buffer, CL_TRUE, 0, sizeof(unsigned int) * 1024, coins, 0, NULL, NULL);

        // Set kernel arguments and run it
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &coins_buffer);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &hashes_buffer);
        clSetKernelArg(kernel, 2, sizeof(unsigned int), &1024);  // Number of coins to process

        size_t global_size = 1024;
        clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
        clFinish(queue);

        // Read results from OpenCL buffer
        clEnqueueReadBuffer(queue, hashes_buffer, CL_TRUE, 0, sizeof(unsigned int) * 1024 * 4, hashes, 0, NULL, NULL);

        // Count valid coins based on the hash
        for (int i = 0; i < 1024; i++) {
            unsigned int hash[4] = {hashes[i * 4], hashes[i * 4 + 1], hashes[i * 4 + 2], hashes[i * 4 + 3]};
            int trailing_zeros = count_trailing_zeros(hash);
            if (trailing_zeros >= 20) {  // Adjust condition based on the required number of trailing zero bits
                valid_coin_count++;
            }
        }
    }

    printf("Found %u valid coins in %u seconds\n", valid_coin_count, duration);

    // Cleanup OpenCL
    cleanup_opencl();
    free(coins);
    free(hashes);

    return 0;
}
