# OpenCL-specific Makefile
CC = gcc
CFLAGS = -Wall
LDFLAGS = -lOpenCL

# Define file names
TARGET = hello_world
SRC = opencl_host.c
KERNEL = opencl_kernel.cl
OBJ = $(SRC:.c=.o)

# Default target: compile and link the OpenCL program
all: $(TARGET)

# Rule to create the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to compile the C source file into an object file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

# Clean up compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Target to run the program after building
run: $(TARGET)
	./$(TARGET)
