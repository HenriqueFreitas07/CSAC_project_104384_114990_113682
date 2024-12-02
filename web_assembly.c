//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_search() --- find DETI coins using md5_cpu()
//

typedef unsigned char u08_t;
typedef unsigned int u32_t;
typedef unsigned long long u64_t;

#include <time.h>
#include <stdio.h>

#include "md5.h"

int main(void)
{
  u32_t idx,coin[13u],hash[4u];
  u64_t n_attempts,n_coins;
  u08_t *bytes;
  clock_t t; 

  bytes = (u08_t *)&coin[0];
  //
  // mandatory for a DETI coin
  //
  bytes[0u] = 'D';
  bytes[1u] = 'E';
  bytes[2u] = 'T';
  bytes[3u] = 'I';
  bytes[4u] = ' ';
  bytes[5u] = 'c';
  bytes[6u] = 'o';
  bytes[7u] = 'i';
  bytes[8u] = 'n';
  bytes[9u] = ' ';
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
  t = clock();
  for(n_attempts = n_coins = 0ull;n_attempts < 10000000ull;n_attempts++)
  {
    //
    // compute MD5 hash
    //

    u32_t a,b,c,d,state[4],x[16];
    # define C(c)         (c)
    # define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
    # define DATA(idx)    coin[idx]
    # define HASH(idx)    hash[idx]
    # define STATE(idx)   state[idx]
    # define X(idx)       x[idx]
      CUSTOM_MD5_CODE();
    # undef C
    # undef ROTATE
    # undef DATA
    # undef HASH
    # undef STATE
    # undef X
    

    //
    // byte-reverse each word (that's how the MD5 message digest is printed...)
    //
    // hash_byte_reverse(hash);
    //
    // count the number of trailing zeros of the MD5 hash
    //
    //  n = deti_coin_power(hash);
    //
    // if the number of trailing zeros is >= 32 we have a DETI coin
    //
    if(hash[3] == 0)
    {
      for (int i = 0; i < 13; i++) 
        printf("%08X%c", coin[i], (i==12) ? '\n' : ' ');
      //save_deti_coin(coin);
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
  t = clock() - t;
  // STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %llu DETI coin%s found in %llu attempt%s (expected %.2f coins) Time (s): %.2f \n",n_coins,(n_coins == 1ull) ? "" : "s",n_attempts,(n_attempts == 1ull) ? "" : "s",(double)n_attempts / (double)(1ull << 32), (double)t/(double)CLOCKS_PER_SEC );
}

