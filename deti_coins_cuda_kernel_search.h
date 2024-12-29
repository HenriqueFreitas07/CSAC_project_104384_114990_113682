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
  
  initialize_cuda(0,"md5_cuda_kernel.cubin", "cuda_md5_kernel", 0, 1024);
  cu_params[0] = &device_data;
  cu_params[1] = &device_hash;

  // cuda_md5_kernel(u32_t *interleaved32_data,u32_t *interleaved32_hash)
  
  u32_t n,idx,coin[13u],hash[4u];
  u64_t n_attempts,n_coins;
  u08_t *bytes;

  bytes = (u08_t *)&coin[0];
  
  //
  // mandatory for a DETI coin
  //
  coin[ 0] = 0x49544544; // ITED
  coin[ 1] = 0x696f6320; // ioc_ 
  coin[ 2] = 0x7343206e; // sC_n
  coin[ 3] = 0x30324341; // 02CA
  coin[ 4] = 0x41203432; // A_42
  coin[ 5] = 0x34314441; // 41DA

  coin[ 6] = 0x08200820; // \bX\bX
  coin[ 7] = 0x08200820; // 
  coin[ 8] = 0x08200820; // 41DA
  coin[ 9] = 0x08200820; // 41DA
  coin[10] = 0x08200820; // 41DA
  coin[11] = 0x08200820; // 41DA

  coin[12] = 0x0A200820; // 41DA

  //
  // arbitrary, but printable utf-8 data terminated with a '\n' is hightly desirable
  //
  for(idx = 10u;idx < 13u * 4u - 1u;idx++)
    bytes[idx] = ' ';
  //
  // mandatory termination
  //
  bytes[13u * 4u -  1u] = '\n';
  //
  // find DETI coins
  //
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts++)
  {
    //
    // compute MD5 hash
    //
    md5_cpu(coin,hash);
    //
    // byte-reverse each word (that's how the MD5 message digest is printed...)
    //
    hash_byte_reverse(hash);
    //
    // count the number of trailing zeros of the MD5 hash
    //
    n = deti_coin_power(hash);
    //
    // if the number of trailing zeros is >= 32 we have a DETI coin
    //
    if(n >= 32u)
    {
      save_deti_coin(coin);
      n_coins++;
    }
    //
    // try next combination (byte range: 0x20..0x7E)
    //
    for(idx = 10u;idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126;idx++)
      bytes[idx] = ' ';
    if(idx < 13u * 4u - 1u)
      bytes[idx]++;
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
