static void deti_coins_opencl_search(void) {
    // OpenCL buffer setup
    cl_mem device_data, device_hash;
    cl_int err;
    size_t global_size = 1024; // Example size
    size_t local_size = 64;    // Example size

    device_data = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE, NULL, &err);
    device_hash = clCreateBuffer(context, CL_MEM_WRITE_ONLY, HASH_SIZE, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_data);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_hash);~

    // This is to execture the kernel

    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
    
    // and here were gonna read the results
    clEnqueueReadBuffer(queue, device_hash, CL_TRUE, 0, HASH_SIZE, host_hash, 0, NULL, NULL);

    printf("OpenCL search completed.\n");
}
