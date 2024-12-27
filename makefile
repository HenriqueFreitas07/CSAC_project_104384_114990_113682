# OpenCL paths, make sure you adjust them according to your setup
OPENCL_INCLUDE = /path/to/opencl/include
OPENCL_LIB = /path/to/opencl/lib
OPENCL_LIBS = -lOpenCL

# Compiler and flags
CC = gcc
CFLAGS = -Wall -O3 -g -fopenmp -I$(OPENCL_INCLUDE)
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

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
