//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CUDA kernel code
//
// md5_cuda_kernel() --- each thread computes the MD5 hash of one message
//
// do not use this directy to search for DETI coins!
//

//
// needed stuff
//

typedef unsigned int u32_t;

#include "md5.h"
#define next_value_to_try(v)                              \
	do                                                      \
{                                                       \
	v++;                                                \
	if ( (v & 0xFFu) == 0x7Fu )                           \
	{                                                   \
		v += 0xA1u;               \
		if (((v >> 8) & 0xFFu ) == 0x7Fu)                 \
		{                                               \
			v += 0xA1u << 8;                             \
			if (((v >> 16) & 0xFFu ) == 0x7Fu)            \
			{                                           \
				v += 0xA1u << 16;                        \
				if (((v >> 24) & 0xFFu ) == 0x7Fu)        \
				v += 0xA1u << 24;                    \
			}                                           \
		}                                               \
	}                                                   \
} while(0)
// the nvcc compiler stores x[] and state[] in registers (constant indices!)
//
// global thread number: n = threadIdx.x + blockDim.x * blockIdx.x
// global warp number: n >> 5
// warp thread number: n & 31
//

extern "C" __global__ __launch_bounds__(128,1) void cuda_md5_kernel(u32_t v1,u32_t v2,u32_t *data_storage_device)
{
	u32_t number_coins=64u;	
	u32_t n,a,b,c,d,state[4],x[16],coin[13u],hash[4u];
	n = (u32_t)threadIdx.x + (u32_t)blockDim.x * (u32_t)blockIdx.x;
	//printf("Thread ID:%\n",n);
	u32_t previous;
	//
	// mandatory for a DETI coin
	//

	coin[ 0u] = 0x49544544; // ITED
	coin[ 1u] = 0x696f6320; // ioc_ 
	coin[ 2u] = 0x7343206e; // sC_n
	coin[ 3u] = 0x30324341; // 02CA
	coin[ 4u] = 0x41203432; // A_42
	coin[ 5u] = 0x34314441; // 41DA
	coin[ 6u] = v1; 
	coin[ 7u] = v2; 
	// insert some complexity with the thread ID 
	coin[ 8u] = 0x20202020; 
	coin[ 8u] += (n%64u)<< 0; n/=64u;
	coin[ 8u] += (n%64u)<< 8; n/=64u;
	coin[ 8u] += (n%64u)<< 16; n/=64u;
	coin[ 8u] += (n%64u)<< 24; n/=64u;
	coin[ 9u] = 0x20202020; 
	coin[ 10u] = 0x20202020; 
	coin[ 11u] = 0x20202020; 
	coin[ 12u] = 0x0A202020; 

	for (n=0u;n< number_coins;n++){

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
		//gets the current idx if it has encoutered a new coin then increments it with an atomic add
		u32_t idx = data_storage_device[0];
		//printf("Coin: %52.52s",(char *)coin);
		if (hash[3] == 0 && idx < 1024 - 13) {
			idx = atomicAdd(&data_storage_device[0], 13); // Atomic index increment
			printf("Coin: %52.52s",(char *)coin);
			for (int j = 0; j < 13; j++) {
				// just to print the founded coin
				data_storage_device[idx + j] = coin[j]; // Copy coin to storage
			}
		}
		for(u32_t offset=9u;offset<13u;offset++){
			// change the values 
			previous=coin[offset-1u];
			if (previous == 0x7F7F7F7Fu)
			{
				next_value_to_try(coin[offset]);
			}

			next_value_to_try(coin[offset]);
		}
	}

}
