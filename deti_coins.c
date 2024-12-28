//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// DETI coins main program (possible solution)
//

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

  //
  // correctness tests (-t command line option)
  //
  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
  {
#ifdef SEARCH_UTILITIES
    test_next_value_to_try_ascii(); // this will help warming up (turbo boost) the processor!
#endif
    all_md5_tests();
    return 0;
  }
  //
  // search for DETI coins (-s command line option)
  //
  if ((argc >= 2 && argc <= 4) && argv[1][0] == '-' && argv[1][1] == 's')
  {
    srand((unsigned int)time(NULL));
    seconds = (argc > 2) ? parse_time_duration(argv[2]) : 1800u;
    if (seconds == 0u)
    {
      fprintf(stderr, "main: bad number of seconds --- format [Nd][Nh][Nm][N[s]], where each N is a number and [] means optional\n");
      exit(1);
    }
    if (seconds < 120u) seconds = 120u;  // At least 2 minutes
    if (seconds > 7200u) seconds = 7200u;  // Max 2 hours
    n_random_words = (argc > 3) ? (u32_t)atol(argv[3]) : 1u;
    if (n_random_words > 9u) n_random_words = 9u;
    stop_request = 0;
    //(void)signal(SIGALRM, alarm_signal_handler);
    //(void)alarm((unsigned int)seconds);

    // Only OpenCL search option now
    switch (argv[1][2])
    {
      case '5':  // OpenCL search option
        printf("searching for %u seconds using OpenCL\n", seconds);
        fflush(stdout);
        deti_coins_opencl_search(n_random_words);  // Invoke OpenCL-based search
        break;
      default:
        printf("main: unknown option '%c'\n", argv[1][2]);
        break;
    }
    return 0;
  }
  return 0;
}
