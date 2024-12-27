__kernel void md5_kernel(__global const unsigned char *data, __global unsigned int *hashes, const unsigned int data_size) {
    unsigned int a, b, c, d;
    unsigned int X[16]; // Array to hold the 16 data blocks

    // Each work-item (thread) will process a different message
    unsigned int global_id = get_global_id(0);
    
    // Load the input message (52 bytes for each message)
    // You can adjust this depending on how you store your data
    for (int i = 0; i < 16; i++) {
        // Load data from the buffer, here assuming messages are packed into 16-blocks
        // Modify according to your data structure
        if (global_id * 64 + i * 4 < data_size) {
            X[i] = ((unsigned int*)data)[global_id * 16 + i];
        } else {
            X[i] = 0; // Padding if necessary
        }
    }

    // Initialize MD5 state
    a = 0x67452301;
    b = 0xEFCDAB89;
    c = 0x98BADCFE;
    d = 0x10325476;

    // Perform MD5 transformations using your existing MD5 macro
    // Assuming you've adapted the macros correctly for OpenCL
    CUSTOM_MD5_CODE();

    // Store the resulting hash in the output buffer
    hashes[global_id * 4] = a;
    hashes[global_id * 4 + 1] = b;
    hashes[global_id * 4 + 2] = c;
    hashes[global_id * 4 + 3] = d;
}
