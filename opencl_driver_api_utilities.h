// this is my attempt of the equivalent from CUDA, which I don't even know if it's necessary

#if USE_OPENCL > 0
#ifndef OPENCL_DRIVER_API_UTILITIES
#define OPENCL_DRIVER_API_UTILITIES

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

//
// CL_CALL --- macro that should be used to call an OpenCL function and to test its return value
//
// it should be used to test the return value of calls such as
//   clGetPlatformIDs(&platform_id, 1, &num_platforms);
//   clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
//
// in these cases, f_name is, respectively, clGetPlatformIDs and clGetDeviceIDs, and args is, respectively,
//   (&platform_id, 1, &num_platforms) and (platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL)
//

#define CL_CALL(f_name, args)                                                                                  \
  do                                                                                                          \
  {                                                                                                           \
    cl_int err = f_name args;                                                                                 \
    if (err != CL_SUCCESS)                                                                                     \
    { /* the call failed, terminate the program */                                                            \
      fprintf(stderr, "" # f_name "() returned %d (file %s, line %d)\n", err, __FILE__, __LINE__);             \
      exit(1);                                                                                                \
    }                                                                                                         \
  }                                                                                                           \
  while (0)



static const char *cl_error_string(cl_int err)
{
  static char error_string[64];
# define CASE(error_code)  case error_code: return "" # error_code;
  switch (err)
  { 
    default: sprintf(error_string, "unknown error code (%d)", err); return(error_string);
    CASE(CL_SUCCESS                          );
    CASE(CL_DEVICE_NOT_FOUND                 );
    CASE(CL_DEVICE_NOT_AVAILABLE             );
    CASE(CL_COMPILER_NOT_AVAILABLE           );
    CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE    );
    CASE(CL_OUT_OF_RESOURCES                 );
    CASE(CL_OUT_OF_HOST_MEMORY               );
    CASE(CL_PROFILING_INFO_NOT_AVAILABLE     );
    CASE(CL_MEM_COPY_OVERLAP                 );
    CASE(CL_IMAGE_FORMAT_MISMATCH            );
    CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED       );
    CASE(CL_BUILD_PROGRAM_FAILURE            );
    CASE(CL_MAP_FAILURE                      );
    CASE(CL_MISALIGNED_SUB_BUFFER_OFFSET     );
    CASE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
    CASE(CL_COMPILE_PROGRAM_FAILURE          );
    CASE(CL_LINKER_NOT_AVAILABLE             );
    CASE(CL_DEVICE_PARTITION_FAILED          );
    CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE    );
  };
# undef CASE
}


// variaveis globaisd

static cl_platform_id platform_id;
static cl_device_id cl_device;
static char device_name[256];
static cl_context cl_context;
static cl_command_queue cl_queue;
static cl_program cl_program;
static cl_kernel cl_kernel;
static u32_t *host_data;
static u32_t *host_hash;
static cl_mem device_data;
static cl_mem device_hash;


// inciar o opencl driver

static void initialize_opencl(int platform_index, const char *opencl_program_file, const char *kernel_name, u32_t data_size, u32_t hash_size)
{

  CL_CALL(clGetPlatformIDs, (0, NULL, &platform_index));
  CL_CALL(clGetPlatformIDs, (platform_index, &platform_id, NULL));

  // was failing to detect device
  CL_CALL(clGetDeviceIDs, (platform_id, CL_DEVICE_TYPE_GPU, 1, &cl_device, NULL));


  CL_CALL(clGetDeviceInfo, (cl_device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL));
  printf("initialize_opencl: OpenCL code running on a %s\n", device_name);

    // a queue é equivalente a uma stream
  cl_context = clCreateContext(NULL, 1, &cl_device, NULL, NULL, NULL);
  cl_queue = clCreateCommandQueue(cl_context, cl_device, 0, NULL);

  //
  // Load OpenCL program from file
  //
  FILE *program_file = fopen(opencl_program_file, "r");
  if (!program_file)
  {
    fprintf(stderr, "Error: Unable to open OpenCL program file\n");
    exit(1);
  }
  fseek(program_file, 0, SEEK_END);
  size_t program_size = ftell(program_file);
  rewind(program_file);

  char *program_buffer = (char *)malloc(program_size);
  fread(program_buffer, 1, program_size, program_file);
  fclose(program_file);

  cl_program = clCreateProgramWithSource(cl_context, 1, (const char **)&program_buffer, &program_size, NULL);

  //
  // Build OpenCL program
  //
  CL_CALL(clBuildProgram, (cl_program, 1, &cl_device, NULL, NULL, NULL));

  //
  // Create OpenCL kernel
  //
  cl_kernel = clCreateKernel(cl_program, kernel_name, NULL);

  //
  // Allocate memory
  //
  if (data_size > 0u)
    host_data = (u32_t *)malloc(data_size * sizeof(u32_t));
  else
    host_data = NULL;

  if (hash_size > 0u)
    host_hash = (u32_t *)malloc(hash_size * sizeof(u32_t));
  else
    host_hash = NULL;

  //
  // Create OpenCL buffer
  //
  if (data_size > 0u)
    device_data = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, data_size * sizeof(u32_t), NULL, NULL);
  if (hash_size > 0u)
    device_hash = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, hash_size * sizeof(u32_t), NULL, NULL);
}


//
// terminate the OpenCL driver API interface
//

static void terminate_opencl(void)
{
  if (host_data != NULL)
  {
    free(host_data);
    clReleaseMemObject(device_data);
  }
  if (host_hash != NULL)
  {
    free(host_hash);
    clReleaseMemObject(device_hash);
  }
  clReleaseKernel(cl_kernel);
  clReleaseProgram(cl_program);
  clReleaseCommandQueue(cl_queue);
  clReleaseContext(cl_context);
}

#endif
#endif
