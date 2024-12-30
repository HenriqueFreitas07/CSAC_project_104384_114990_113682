# Paths to OpenCL SDK (assuming CUDA)
OPENCL_INC = /usr/local/cuda/include
OPENCL_LIB = /usr/local/cuda/lib64
OPENCL_LIB_NAME = OpenCL

# Source files
C_SRCS = deti_coins.c
CL_SRC = md5_opencl_kernel.cl

# Output binary
OUT_BIN = deti_coins_opencl

# Compiler and linker
CC = gcc
CFLAGS = -I$(OPENCL_INC) -O2 -Wall
LDFLAGS = -L$(OPENCL_LIB) -l$(OPENCL_LIB_NAME)

# Compiler for OpenCL
CL_COMPILER = clang

# Targets
all: $(OUT_BIN)

$(OUT_BIN): $(C_SRCS) $(CL_SRC)
	$(CC) $(CFLAGS) -o $(OUT_BIN) $(C_SRCS) $(LDFLAGS)

# Rule to compile .cl files (OpenCL kernels)
%.bin: %.cl
	$(CL_COMPILER) -cl-fast-relaxed-math -DCL_VERSION_1_2 -cl-kernel-options=-cl-std=CL1.2 -o $@ $<

clean:
	rm -f deti_coins_opencl *.bin

.PHONY: all clean
