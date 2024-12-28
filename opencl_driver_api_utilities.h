#include <CL/cl.h>

#define CL_CALL(f_name, args)                                        \
  do {                                                               \
    cl_int err = f_name args;                                        \
    if (err != CL_SUCCESS) {                                         \
      fprintf(stderr, #f_name "() returned error code %d (file %s, line %d)\n", err, __FILE__, __LINE__); \
      exit(1);                                                        \
    }                                                                \
  } while (0)
