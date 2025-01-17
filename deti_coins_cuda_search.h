#ifndef DETI_COINS_CUDA_SEARCH
#define DETI_COINS_CUDA_SEARCH
#include "deti_coins_cpu_special_search.h"
#include <time.h>

long calculate_elapsed_time(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}
u32_t next_value_to_try_ascii(u32_t v){
  v++;                                                
  if ( (v & 0xFF) == 0x7F )                           
  {                                                   
    v += 0xA1; 
    if (((v >> 8) & 0xFF ) == 0x7F)                 
    {                                               
      v += 0xA1 << 8;                             
      if (((v >> 16) & 0xFF ) == 0x7F)            
      {                                           
        v += 0xA1 << 16;                        
        if (((v >> 24) & 0xFF ) == 0x7F)        
          v += 0xA1 << 24;                    
      }                                           
    }                                               
  }                                                   
  return v; 
}
#define AVG_10 40;
extern int histogram[AVG_10]={0};

void deti_coins_cuda_search(u32_t random_words) {
  u32_t block_size = 128;
  // u32_t number_coins_per_thread = 64u;
  void *cu_params[4];   
  struct timespec start_time, end_time;
  u32_t size = 1024u;
  // Initialize CUDA resources
  u64_t n_attempts = 0, n_coins = 0, kernel_calls = 0, sum_times_ns = 0;
  u32_t idx,max_idx,custom_word1,custom_word2;
  custom_word1=custom_word2=0x20202020;

  initialize_cuda(0, "md5_cuda_kernel.cubin", "cuda_md5_kernel",size,0u);
  max_idx=1u;
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=(64ul<<20ul))
  {

    host_data[0]=1u;
    CU_CALL(cuMemcpyHtoD, (device_data, (void *)host_data, (size_t)size * sizeof(u32_t)));
    // Set kernel parameters
    cu_params[0] = &custom_word1;                        // v1
    cu_params[1] = &custom_word2;                        // v2
    //cu_params[2] = &number_coins_per_thread;   // number of coins per thread
    cu_params[2] = &device_data;               // device data
    //count when the kernel is launch
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Launch the CUDA kernel
    CU_CALL(cuLaunchKernel,
            (cu_kernel, 
            (1 << 20) / block_size, 1, 1, 
            block_size, 1, 1, 
            0, 
            (CUstream)0, 
            &cu_params[0], 
            NULL));
    //count when the kernel has done computing
    clock_gettime(CLOCK_MONOTONIC, &end_time);


    CU_CALL(cuMemcpyDtoH, (host_data, device_data,(size_t)size * sizeof(u32_t)));
    if(host_data[0] > max_idx)
      max_idx=host_data[0];
    //collects the generated coins 
    for(idx=1u;idx<host_data[0] && idx <= size - 13u;idx+=13)
    {
      if(idx <= size -13)
      {
        save_deti_coin(&host_data[idx]);
        n_coins++;
      }
      else
      fprintf(stderr,"deti_coins_cuda_search: wasted DETI coin\n");
    }
    if(custom_word1 != 0x7E7E7E7Eu)	
      custom_word1=next_value_to_try_ascii(custom_word1);
    else
    {
      custom_word1=0x20202020u;
      custom_word2=next_value_to_try_ascii(custom_word2);
    }
    long elapsed_time_ns = calculate_elapsed_time(start_time, end_time);
    long hist_elapsed = elapsed_time_ns * 10;
    //histograma
    histogram[hist_elapsed]+=1;
  }
  // Calculate average kernel execution time in microseconds
  for (int i=0; i< AVG_10; i++) {
    printf("For all the kernels with %l \n",histogram[i]);
  }
  STORE_DETI_COINS();	
  printf("deti_coins_cuda_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
         n_coins, 
         (n_coins == 1ul) ? "" : "s", 
         n_attempts, 
         (n_attempts == 1ul) ? "" : "s", 
         (double)n_attempts / (double)(1ul << 32));
  terminate_cuda();
}

#endif // DETI_COINS_CUDA_KERNEL_SEARCH
