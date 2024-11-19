// a useful data type

typedef union 
{
    u32_t coin_as_ints[13]; // 13 4-byte integers
    char coin_as_chars[52]; // 52=13*4 bytes
}
coin_t;

// to initialize the coin as a string ...

coin_t coin;
int n;
if((n = snprintf(coin.coin_as_chars, 52, "DETI coin .....\n" )) != 52) 
{
    fprintf(stderr, "not exactly 52 bytes... (n=%d)\n", n);
    exit(1);
}

// to read it as integers...
for(idx=0; idx < 13; idx++) 
    ... coin.coin_as_ints[idx] ...