# ----------------------------
# Makefile with build directory
# ----------------------------

# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Target executable
TARGET = build/main

# Source files
SRCS = main.cpp audio_io.cpp

# Object files in build/ directory
OBJS = $(patsubst %.cpp,build/%.o,$(SRCS))

# Ensure build directory exists
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compile .cpp files to .o in build/
build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)
