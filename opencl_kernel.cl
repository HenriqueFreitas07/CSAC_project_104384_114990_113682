// opencl_kernel.cl
__kernel void hello_world(__global char* output) {
    size_t id = get_global_id(0);  // Get the global index of the work-item

    output[id] = 'H' + id;
}
