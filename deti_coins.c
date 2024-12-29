#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef _WIN32
    // Windows-specific code or libraries
    #include <windows.h>
#else
    // Unix-like system specific code
    #include <sys/resource.h>
#endif
// #include <mpi.h>
#include "deti_coins_opencl.h"  // Include OpenCL header

//
// number of threads
//

#ifndef n_threads
# define n_threads   4
#endif
#if n_threads < 2 || n_threads > 16
# error "too few or too many threads"
#endif
#define n_threads_max  16


#if defined(__GNUC__) && __BYTE__ORDER__ != __LITTLE_ENDIAN__
# error "this code requires a little-endian processor"
#endif

#ifndef USE_CUDA
# define USE_CUDA 0
#endif


//
// unsigned integer data types and some useful functions (in cpu_utilities.h)
//

// typedef unsigned char u08_t;
typedef uint8_t u08_t;
typedef unsigned  int u32_t;
typedef unsigned long u64_t;

#include "cpu_utilities.h"


//
// MD5 hash implementations and respective tests
//

#include "md5.h"
#include "md5_test_data.h"
#include "md5_cpu.h"
#include "md5_cpu_avx.h"
//#include "md5_cpu_avx2.h"
#include "md5_cpu_neon.h"
#if USE_CUDA > 0
# include "cuda_driver_api_utilities.h"
# include "md5_cuda.h"
#endif

static void all_md5_tests(void)
{
  //
  // make random test data
  //
  make_random_md5_test_data();
  //
  // any: md5_cpu() tests
  //
  test_md5_cpu();
  //
  // intel/amd: md5_cpu_avx() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_AVX
  test_md5_cpu_avx();
#endif
  //
  // intel/amd: md5_cpu_avx2() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_AVX2
  test_md5_cpu_avx2();
#endif
  //
  // arm: md5_cpu_neon() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_NEON
  test_md5_cpu_neon();
#endif
  //
  // cuda: md5_cuda() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CUDA
  test_md5_cuda();
#endif
}

//
// saving are reporting DETI coins
//

#include "deti_coins_vault.h" 


//
// search for DETI coins
//

static volatile int stop_request;

//
// Commenting out unused function `alarm_signal_handler`
// This function was not used anywhere in the code
//

// static void alarm_signal_handler(int dummy)
// {
//   stop_request = 1;
// }

#include "search_utilities.h"
#ifdef MD5_CPU_AVX
# include "deti_coins_cpu_avx_search.h"
#endif

//#ifdef MD5_CPU_AVX2
//# include "deti_coins_cpu_avx2_search.h"
//#endif
//#ifdef MD5_CPU_NEON
//# include "deti_coins_cpu_neon_search.h"
//#endif
//#if USE_CUDA > 0
//# include "deti_coins_cuda_search.h"
//#endif

#ifdef DETI_COINS_OPENCL_SEARCH
#include "deti_coins_opencl.h"  // Include OpenCL search header
#endif

//
// Commenting out unused function `deti_coins_cpu_special_search`
// This function was not used anywhere in the code
//

// static void deti_coins_cpu_special_search(void)
// {
//   // Function implementation
// }

//
// main program
//

int main(int argc, char **argv)
{
    u32_t seconds, n_random_words;

    // Debug print to ensure arguments are correctly received
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    // Correctness tests (-t command line option)
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
    {
#ifdef SEARCH_UTILITIES
        test_next_value_to_try_ascii(); // this will help warming up (turbo boost) the processor!
#endif
        all_md5_tests();
        return 0;
    }

    // Search for DETI coins (-s command line option)
    if ((argc >= 2 && argc <= 4) && argv[1][0] == '-' && argv[1][1] == 's')
    {
        // Ensure sufficient arguments
        if (argc < 4) {
            printf("main: insufficient arguments for -s option. Usage: -s <seconds> <random_words>\n");
            return 1;
        }

        // Parse seconds and random words from command-line arguments
        // seconds = (u32_t)atoi(argv[2]);  // Convert seconds argument to integer
        seconds = parse_time_duration(argv[2]);  // Use the parse_time_duration function

        n_random_words = (u32_t)atoi(argv[3]);  // Convert random words argument to integer

        if (seconds == 0u) {
            printf("main: bad number of seconds --- format [Nd][Nh][Nm][N[s]], where each N is a number and [] means optional\n");
            return 1;
        }

        // Ensure seconds is within a reasonable range
        if (seconds < 120u) seconds = 120u;  // At least 2 minutes
        if (seconds > 7200u) seconds = 7200u;  // Max 2 hours

        // Debug print to ensure this block is being reached
        printf("Starting search for DETI coins for %u seconds with %u random words...\n", seconds, n_random_words);

        stop_request = 0;

        // OpenCL Debugging Step: Print device information
        cl_uint num_devices;
        cl_int err;
        err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get device IDs (%d)\n", err);
            return 1;
        }
        printf("OpenCL Devices Found: %u\n", num_devices);

        // Debugging: Show OpenCL platform and device information
        cl_platform_id platform_id;
        err = clGetPlatformIDs(1, &platform_id, NULL);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get platform IDs (%d)\n", err);
            return 1;
        }

        char platform_name[128];
        err = clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get platform name (%d)\n", err);
            return 1;
        }
        printf("OpenCL Platform: %s\n", platform_name);

        cl_device_id device_id;
        err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL); // Get first GPU device
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get device ID (%d)\n", err);
            return 1;
        }

        // OpenCL Debugging: Check the device's name
        char device_name[128];
        err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get device name (%d)\n", err);
            return 1;
        }
        printf("OpenCL Device: %s\n", device_name);

        // OpenCL Debugging: Check the device capabilities
        cl_uint compute_units;
        err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to get compute units info (%d)\n", err);
            return 1;
        }
        printf("OpenCL Device Compute Units: %u\n", compute_units);

        // Now, call the OpenCL search function
        deti_coins_opencl_search(n_random_words);  // Invoke OpenCL-based search
        return 0;
    }

    // Default case for other arguments
    printf("No valid command-line options passed.\n");
    return 0;
}
