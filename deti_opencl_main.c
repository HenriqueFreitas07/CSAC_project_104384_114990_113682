#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "deti_opencl.h"

// Function to set up OpenCL and start the mining process
int main(int argc, char **argv)
{
    unsigned int seconds = 1800;  // Run for 30 minutes by default
    
    // Initialize OpenCL and mining parameters
    printf("Starting DETI Coin Mining using OpenCL...\n");
    
    // Call the mining function that will use OpenCL
    deti_opencl_opencl(seconds);
    
    return 0;
}
