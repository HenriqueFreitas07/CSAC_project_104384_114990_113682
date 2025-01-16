//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_special_search() --- find DETI coins using md5_cpu()
//

#ifndef DETI_COINS_CPU_AVX2_SEARCH
#define DETI_COINS_CPU_AVX2_SEARCH


static void deti_coins_cpu_avx2_search(u32_t n_random_words)
{
# define N_LANES 8u
  u32_t coins[13u*N_LANES],hash[4u * N_LANES],coin[13];
  u64_t n_attempts,n_coins;
  
  // ! The insert is made in reverse order because it's little endian
  // We want to start writing the integer in the most significant byte
  // We also need to do this because md5 works with integers

  // More efficient way to do the above
  // "DETI coin" in hexadecimal
  // 0x44 0x45 0x54 0x49 0x20 0x63 0x6f 0x69 0x6e 0x20

  //
  // find DETI coins
  //

    for (u32_t lane= 0u; lane < N_LANES; lane++) {
        
        coins[ 0 * N_LANES + lane] = 0x49544544; // ITED
        coins[ 1 * N_LANES + lane] = 0x696f6320; // ioc_ 
        coins[ 2 * N_LANES + lane] = 0x7343206e; // sC_n
        coins[ 3 * N_LANES + lane] = 0x30324341; // 02CA
        coins[ 4 * N_LANES + lane] = 0x41203432; // A_42
        coins[ 5 * N_LANES + lane] = 0x34314441; // 41DA
        coins[ 6 * N_LANES + lane] = 0x08200841 + (lane); // \bX\bX
        coins[ 7 * N_LANES + lane] = 0x08200820; // 
        coins[ 8 * N_LANES + lane] = 0x08200820; // 41DA
        coins[ 9 * N_LANES + lane] = 0x08200820; // 41DA
        coins[10 * N_LANES + lane] = 0x08200820; // 41DA
        coins[11 * N_LANES + lane] = 0x08200820; // 41DA
        coins[12 * N_LANES + lane] = 0x0A200820; // 41DA
    }
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=N_LANES)
  {

    //
    // compute MD5 hash
    //
    md5_cpu_avx2((v8si *)coins,(v8si *)hash);
    //
    //
    //
    for (u32_t lane= 0u; lane < N_LANES; lane++)
    {
      if(hash[3 * N_LANES + lane] == 0)
      {
        for(u32_t idx = 0;idx < 13;idx++) coin[idx] = coins[idx * N_LANES + lane];
        save_deti_coin(coin);
        n_coins++;
        printf("FOUND ONE: %52.52s",(char *)coin);
        fflush(stdout);
      }
    }
    //
    // try next combination (byte range: 0x20..0x7E)
    //
    u32_t idx = 6u;    
    if((coins[idx*N_LANES + 0] & 0x00FF0000 ) != 0x007E0000)
       coins[idx*N_LANES + 0] += 0x00010000;
    else
    {
      coins[idx*N_LANES + 0] += 0xFFA20000; // UV7EWXYZ + FFA20000 = UV20WXYZ
      for(idx = 7;idx < 12;idx++)
        {
          if((coins[idx*N_LANES + 0] & 0x000000FF) != 0x0000007E)
          {
            coins[idx*N_LANES + 0] += 0x00000001;
            break;
          }
          if((coins[idx*N_LANES + 0] & 0x00FF0000) != 0x007E0000)
          {
            coins[idx*N_LANES + 0] += 0x0000FFA2;
            break;
          }
          coins[idx*N_LANES + 0] += 0xFFA1FFA2;
        }
    }
    for(;idx > 6;idx--)
      for (u32_t lane = 1; lane < N_LANES; lane++)
        coins[idx*N_LANES + lane] = coins[idx*N_LANES + 0];
    for (u32_t lane = 1; lane < N_LANES; lane++)
      coins[idx*N_LANES + lane] = coins[idx*N_LANES + 0] + lane;
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_avx2_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
# undef N_LANES
}
#endif
