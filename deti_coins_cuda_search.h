#ifndef DETI_COINS_CUDA_SEARCH
#define DETI_COINS_CUDA_SEARCH
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

void deti_coins_cuda_search(u32_t random_words) {
    u32_t block_size = 128;
    // u32_t number_coins_per_thread = 64u;
    void *cu_params[4];

    u32_t size = 1024u;
    // Initialize CUDA resources
    u32_t n_attempts = 0, n_coins = 0;
    u32_t idx,max_idx,custom_word1,custom_word2;
    custom_word1=custom_word2=0x20202020;

    initialize_cuda(0, "md5_cuda_kernel.cubin", "cuda_md5_kernel",size,0u);
    max_idx=1u;
   for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=(1<<20)*64u)
   {

	host_data[0]=1u;
        CU_CALL(cuMemcpyHtoD, (device_data, (void *)host_data, (size_t)size * sizeof(u32_t)));
        // Set kernel parameters
        cu_params[0] = &custom_word1;                        // v1
        cu_params[1] = &custom_word2;                        // v2
        //cu_params[2] = &number_coins_per_thread;   // number of coins per thread
        cu_params[2] = &device_data;               // device data

        // Launch the CUDA kernel
        CU_CALL(cuLaunchKernel,
                (cu_kernel, 
                 (1 << 20) / block_size, 1, 1, 
                 block_size, 1, 1, 
		 0, 
		 (CUstream)0, 
		 &cu_params[0], 
		 NULL));

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

   }

   STORE_DETI_COINS();	
   printf("deti_coins_cuda_search: %u DETI coin%s found in %u attempt%s (expected %.2f coins)\n",
		   n_coins, 
		   (n_coins == 1ul) ? "" : "s", 
		   n_attempts, 
		   (n_attempts == 1ul) ? "" : "s", 
		   (double)n_attempts / (double)(1ul << 32));
   terminate_cuda();
}

#endif // DETI_COINS_CUDA_KERNEL_SEARCH
