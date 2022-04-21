#==========================================================
#	 Environment setup
#==========================================================

# Compilers
CC := gcc
CXX := g++
AS := as

# Project structures
BIN_DIR := bin
BUILD_DIR := build
SRC_DIRS := kernel libcpp
INC_DIRS := $(SRC_DIRS)
# INC_FLAGS := $(addprefix -I,$(INC_DIRS))
KERNEL_BIN := $(BIN_DIR)/kernel.bin
OS_IMAGE := $(BIN_DIR)/myos.iso

# C specifics
C_SOURCES := $(shell find $(SRC_DIRS) -mindepth 1 -type f -name *.c)
C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))

# C++ specifics
CXX_SOURCES := $(shell find $(SRC_DIRS) -mindepth 1 -type f -name *.cpp)
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(CXX_SOURCES:.cpp=.o))

# ASM specifics
# AS_SOURCES := $(shell find . -mindepth 1 -type f -name *.s)
# AS_OBJECTS := $(patsubst ./%,$(BUILD_DIR)/%,$(AS_SOURCES:.s=.o))

# Aggregation of all the objects
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)

# Delete targets in case of error
.DELETE_ON_ERROR:

# Disable invocation echoing
.SILENT:

# Disable implicit rules (faster exec + easier Makefile debugging)
MAKEFLAGS += --no-builtin-rules


#==========================================================
#	 Special flags
#==========================================================

# Compiler flags
C_CXX_FLAGS := $(shell cat compile_flags.txt | tr '\n' ' ')
CFLAGS := $(C_CXX_FLAGS) -std=c99
CXXFLAGS := $(C_CXX_FLAGS) -std=c++17 -fno-exceptions -fno-rtti
ASFLAGS := -32

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L. 
# Libraries (-lfoo) should be added to the LDLIBS variable instead. 
# LDFLAGS :=
# Library flags or names given to compilers when they are supposed to invoke the linker, ‘ld’. 
# Non-library linker flags, such as -L, should go in the LDFLAGS variable
# LDLIBS :=


#==========================================================
#	 Default rule
#==========================================================

.PHONY: all
all: run


#==========================================================
#	 Regular build
#==========================================================

# Build  the  kernel  binary
$(KERNEL_BIN): $(OBJECTS) $(BUILD_DIR)/boot/boot.o
	mkdir -p $(@D)
	echo "    \e[95m[binary]\e[0m $@"
	$(CC) $(CFLAGS) -T boot/linker.ld -o $@ $^

$(BUILD_DIR)/boot/boot.o: boot/boot.s
	mkdir -p $(@D)
	echo "    \e[96m[object]\e[0m $<"
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	echo "    \e[96m[object]\e[0m $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	echo "    \e[96m[object]\e[0m $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<


#==========================================================
#	 Run
#==========================================================

.PHONY: run
run: $(KERNEL_BIN)
	echo "    \e[92m[run]   \e[0m $<"
	qemu-system-i386 -kernel $<


#==========================================================
#	 Clean
#==========================================================

.PHONY: clean
clean:
	echo "    \e[91m[delete]\e[0m build"
	rm -rf build
	echo "    \e[91m[delete]\e[0m bin"
	rm -rf bin







