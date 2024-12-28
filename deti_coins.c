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
#include <sys/resource.h>
#include <mpi.h>
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

typedef unsigned char u08_t;
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

static void alarm_signal_handler(int dummy)
{
  stop_request = 1;
}

#include "deti_coins_cpu_search.h"
#include "deti_coins_cpu_special_search.h"
#include "deti_coins_cpu_OpenMP_search.h"
#include "deti_coins_cpu_MPI_OpenMP_search.h"

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
    srandom((unsigned int)time(NULL));
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
    (void)signal(SIGALRM, alarm_signal_handler);
    (void)alarm((unsigned int)seconds);

    switch (argv[1][2])
    {
      case '5':  // OpenCL search option
        printf("searching for %u seconds using OpenCL\n", seconds);
        fflush(stdout);
        deti_coins_opencl_search(n_random_words);  // Invoke OpenCL-based search
        break;
      case '0':
        printf("searching for %u seconds using deti_coins_cpu_search()\n", seconds);
        fflush(stdout);
        deti_coins_cpu_search();
        break;
#ifdef DETI_COINS_CPU_AVX_SEARCH
      case '1':
        printf("searching for %u seconds using deti_coins_cpu_avx_search()\n", seconds);
        fflush(stdout);
        deti_coins_cpu_avx_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CPU_OpenMP_SEARCH
      case '7':
        printf("searching for %u seconds using deti_coins_cpu_OpenMP_search()\n", seconds);
        fflush(stdout);
        
        unsigned long total_coins = 0;
        unsigned long total_attempts = 0;

        // Parallel region with reduction to sum the totals across threads
        # pragma omp parallel num_threads(n_threads) reduction(+:total_coins, total_attempts)
          { // automatic variable are local to the thread
            int thread_number = omp_get_thread_num(); 
            unsigned long n_coins = 0;
            unsigned long n_attempts = 0;

            deti_coins_cpu_OpenMP_search(thread_number, &n_coins, &n_attempts);
            
            total_coins += n_coins;
            total_attempts += n_attempts;
          }
        
        // Final aggregated results
        double total_expected_coins = (double)total_attempts / (1ul << 32);
        printf("Total: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
              total_coins, (total_coins == 1ul) ? "" : "s", total_attempts,
              (total_attempts == 1ul) ? "" : "s", total_expected_coins);

        STORE_DETI_COINS();  
        break;
#endif
#ifdef DETI_COINS_CPU_MPI_OpenMP_SEARCH
      case '8':
        int n_processes, rank;

        //
        // initialize the MPI environment, and get the number of processes and the MPI number of our process (the rank)
        //
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
        printf("Process %d/%d: searching for DETI coins\n", rank, n_processes);
        fflush(stdout);

        // Local variables for this MPI process
        unsigned long local_coins = 0;
        unsigned long local_attempts = 0;

        // Parallel region with reduction to sum the totals across threads
        #pragma omp parallel num_threads(n_threads) reduction(+:local_coins, local_attempts)
        {
            int thread_number = omp_get_thread_num();
            unsigned long n_coins = 0;
            unsigned long n_attempts = 0;

            deti_coins_cpu_MPI_OpenMP_search(rank, thread_number, &n_coins, &n_attempts); // TODO: create deti_coins_cpu_MPI_OpenMP_search

            local_coins += n_coins;
            local_attempts += n_attempts;
        }

        // Each process stores its own coins locally
        STORE_DETI_COINS(); // Store the coins found by this process
        
        // Global variables for total aggregation
        unsigned long global_coins = 0;
        unsigned long global_attempts = 0;

        // Use MPI_Reduce to aggregate results from all processes to rank 0
        MPI_Reduce(&local_coins, &global_coins, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_attempts, &global_attempts, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

        // Rank 0 prints the global aggregated results
        if (rank == 0) {
            double total_expected_coins = (double)global_attempts / (1ul << 32);
            printf("Total: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
                  global_coins, (global_coins == 1ul) ? "" : "s", global_attempts,
                  (global_attempts == 1ul) ? "" : "s", total_expected_coins);

            // STORE_DETI_COINS(); // Store global results
        }

        MPI_Finalize();
        break;
#endif
#ifdef DETI_COINS_CPU_AVX2_SEARCH
      case '2':
        printf("searching for %u seconds using deti_coins_cpu_avx2_search()\n", seconds);
        fflush(stdout);
        deti_coins_cpu_avx2_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CPU_NEON_SEARCH
      case '3':
        printf("searching for %u seconds using deti_coins_cpu_neon_search()\n", seconds);
        fflush(stdout);
        deti_coins_cpu_neon_search(n_random_words);
        break;
#endif
#if USE_CUDA > 0
      case '4':
        printf("searching for %u seconds using deti_coins_cuda_search()\n", seconds);
        fflush(stdout);
        deti_coins_cuda_search(n_random_words);
        break;
#endif
      default:
        fprintf(stderr, "main: unknown option: %s\n", argv[1]);
        exit(1);
    }

    return 0;
  }

  fprintf(stderr, "usage: %s -s [seconds] [random_words]  or  %s -t\n", argv[0], argv[0]);
  return 1;
}
