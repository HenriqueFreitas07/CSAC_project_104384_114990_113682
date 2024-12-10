

void deti_coins_cuda_search(u32_t random_words){
  initialize_cuda(0,"cuda_md5_kernel.cubin","cuda_md5_kernel",0u,1024u);
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=64)
  {
    //
    CU_CALL( cuMemcpyHtoD , (device_data,(void *)host_data,(size_t)n_messages * (size_t)13 * sizeof(u32_t)) );
    cu_params[0] = &device_data;
    cu_params[1] = &device_hash;
    CU_CALL( cuLaunchKernel , (cu_kernel,n_messages / 128u,1u,1u,128u,1u,1u,0u,(CUstream)0,&cu_params[0],NULL) );
     
    CU_CALL( cuMemcpyDtoH , ((void *)host_hash,device_hash,(size_t)n_messages * (size_t)4 * sizeof(u32_t)) );
    CU_CALL( cuStreamSynchronize , (0) );
  printf("deti_coins_cpu_avx2_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
# undef N_LANES



}
