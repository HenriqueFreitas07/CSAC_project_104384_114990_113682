# Paths to OpenCL SDK (assuming CUDA)
OPENCL_INC = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include"
OPENCL_LIB = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/lib/x64"
OPENCL_LIB_NAME = OpenCL.lib  # The actual OpenCL library name

# Source files
C_SRCS = main.c
CL_SRC = md5_opencl_kernel.cl

# Output binary
OUT_BIN = deti_coins_opencl.exe

# Compiler and linker
CC = gcc   # Or use cl if using MSVC
CFLAGS = -I$(OPENCL_INC) -O2 -Wall
LDFLAGS = -L$(OPENCL_LIB) -l$(OPENCL_LIB_NAME)

# Compiler for OpenCL
CL_COMPILER = cl

# Targets
all: $(OUT_BIN)

$(OUT_BIN): $(C_SRCS) $(CL_SRC)
	$(CC) $(CFLAGS) -o $(OUT_BIN) $(C_SRCS) $(LDFLAGS)

# Rule to compile .cl files (OpenCL kernels)
%.bin: %.cl
	$(CL_COMPILER) -cl-fast-relaxed-math -DCL_VERSION_1_2 -cl-kernel-options=-cl-std=CL1.2 -o $@ $<

clean:
	rm -f $(OUT_BIN) *.bin

.PHONY: all clean
