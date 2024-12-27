# OpenCL paths (adjust for your actual installation)
OPENCL_INCLUDE = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include"
OPENCL_LIB = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/lib/x64"
OPENCL_LIBS = -lOpenCL  # For Windows, it is typically 'OpenCL.lib'

# Compiler and flags
CC = gcc
CFLAGS = -Wall -O3 -g -fopenmp -I$(OPENCL_INCLUDE)  # Correct this to include the full OpenCL path
LDFLAGS = -L$(OPENCL_LIB) $(OPENCL_LIBS)

# Source files
SRCS = md5_opencl_host.c opencl_md5_kernel.cl
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = mine_deti_coins

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile OpenCL source to object file
md5_opencl_host.o: md5_opencl_host.c
	$(CC) $(CFLAGS) -c md5_opencl_host.c

# Clean up
clean:
	del /f /q $(OBJS) $(TARGET)

.PHONY: all clean
