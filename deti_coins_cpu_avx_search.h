//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_special_search() --- find DETI coins using md5_cpu()
//

#ifndef DETI_COINS_CPU_SPECIAL_SEARCH
#define DETI_COINS_CPU_SPECIAL_SEARCH


static void deti_coins_cpu_special_search(void)
{
# define N_LANES 4u
  u32_t n,idx,coin[13u*N_LANES],hash[4u * N_LANES];
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
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=4)
  {
    for (u32_t lane= 0u; lane < N_LANES; lane++) {
        
        coin[ 0 * 4 + lane] = 0x49544544; // ITED
        coin[ 1 * 4 + lane] = 0x696f6320; // ioc_ 
        coin[ 2 * 4 + lane] = 0x7343206e; // sC_n
        coin[ 3 * 4 + lane] = 0x30324341; // 02CA
        coin[ 4 * 4 + lane] = 0x41203432; // A_42
        coin[ 5 * 4 + lane] = 0x34314441; // 41DA
        coin[ 6 * 4 + lane] = 0x08200848 + (lane<<24); // \bX\bX
        coin[ 7 * 4 + lane] = 0x08200820; // 
        coin[ 8 * 4 + lane] = 0x08200820; // 41DA
        coin[ 9 * 4 + lane] = 0x08200820; // 41DA
        coin[10 * 4 + lane] = 0x08200820; // 41DA
        coin[11 * 4 + lane] = 0x08200820; // 41DA

        coin[12+(i*13u)] = 0x0A200820; // 41DA
      
    }
 // printf("%52.52s",(char *)coin);
    //
    // compute MD5 hash
    //
    md5_cpu_avx(coin,hash);
    //
    //
    //
    if(hash[3] == 0)
    {
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
        printf("FOUND ONE: %52.52s",(char *)coin);
        fflush(stdout);
      }
    }
    //
    // try next combination (byte range: 0x20..0x7E)
    //
    for(idx = 6;idx < 12;idx++)
    {
      if((coin[idx] & 0x000000FF) != 0x0000007E) { coin[idx] += 0x00000001; break; }
      if((coin[idx] & 0x00FF0000) != 0x007E0000) { coin[idx] += 0x0000FFA2; break; }
      coin[idx] += 0xFFA1FFA2;
    }
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
# undef N_LANES
}
#endif
