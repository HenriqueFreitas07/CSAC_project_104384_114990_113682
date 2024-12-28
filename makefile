# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -std=c++11 -Wall

# OpenCL flags (path to OpenCL header and library directories)
OPENCL_INC = C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.6\include
OPENCL_LIB = C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.6\lib\x64

# OpenCL runtime libraries
OPENCL_LIBS = -lOpenCL

# Source and object files
SRC_FILES = main.cpp md5_opencl.cpp opencl_driver_api_utilities.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Kernel file
KERNEL_FILE = md5_opencl_kernel.cl

# Output binary
OUTPUT = deti_miner.exe

# Default target
all: $(OUTPUT)

# Rule to compile the program
$(OUTPUT): $(OBJ_FILES) $(KERNEL_FILE)
	$(CXX) $(OBJ_FILES) -o $(OUTPUT) $(OPENCL_LIBS)

# Compile the OpenCL-related C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(OPENCL_INC) -c $< -o $@

# Rule to clean the build
clean:
	del /f /q $(OBJ_FILES) $(OUTPUT)

# Rule to print the paths and check the setup
print:
	@echo "OpenCL Include Path: $(OPENCL_INC)"
	@echo "OpenCL Library Path: $(OPENCL_LIB)"
	@echo "Kernel File: $(KERNEL_FILE)"
