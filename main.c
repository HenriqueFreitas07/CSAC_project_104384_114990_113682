#include <stdio.h>
#include "deti_coins_opencl.h"

// The entry point of the program
int main() {
    // OpenCL kernel source, which is typically loaded from a file
    const char *kernel_source = "md5_opencl_kernel.cl";  // Adjust the path if needed

    // Initialize OpenCL (this function initializes context, device, etc.)
    initialize_opencl(kernel_source);
    
    // Start the search function (this might use the OpenCL kernel)
    deti_coins_opencl_search(1000);  // The parameter could be adjusted as needed
    
    // Clean up OpenCL resources
    cleanup_opencl();

    return 0;
}
