//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cuda_kernel_search() --- find DETI coins using md5_cpu()
//

#ifndef DETI_COINS_CUDA_KERNEL_SEARCH
#define DETI_COINS_CUDA_KERNEL_SEARCH

static void deti_coins_cuda_kernel_search(void)
{
  u32_t v1= 0x20202020;
  u32_t v2= 0x20202020;
  //
  u32_t block_size = 128

  initialize_cuda(0,"md5_cuda_kernel.cubin", "cuda_md5_kernel", 0, 1024);
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=(1<<20)*64)
  {
   next_value_to_try(v1);
   if(v1 == 0x20202020) 
       next_value_to_try(v2);
    //v1
    cu_params[0] = &v1;
    //v2
    cu_params[1] = &v2;
    // hash array
    device_data[0]=1
    cu_params[2]= &device_data;
    CU_CALL( cuLaunchKernel , (cu_kernel,(1<<20)/block_size,1,1,block_size,1,1,0,(CUstream)0,&cu_params[0],NULL) 

    if(n >= 32u)
    {
      save_deti_coin(coin);
      n_coins++;
    }
  }
  terminate_cuda()
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
