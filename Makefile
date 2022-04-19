#==========================================================
#	 Environment setup
#==========================================================

# Compilers
CC := gcc
CXX := g++
AS := nasm

# Project structures
BIN_DIR := bin
BUILD_DIR := build
SRC_DIR := kernel
KERNEL_BIN := $(BIN_DIR)/kernel.bin
OS_IMAGE := $(BIN_DIR)/myos.iso

# C specifics
C_SOURCES := $(shell find $(SRC_DIR) -type f -name *.c)
C_OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(C_SOURCES:.c=.o))

# C++ specifics
CXX_SOURCES := $(shell find $(SRC_DIR) -type f -name *.cpp)
CXX_OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(CXX_SOURCES:.cpp=.o))

# ASM specifics
# AS_SOURCES := $(shell find . -type f -name *.s)
# AS_OBJECTS := $(patsubst ./%,$(BUILD_DIR)/%,$(AS_SOURCES:.s=.o))

# Aggregation of all the objects
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS) $(AS_OBJECTS)

# Delete targets in case of error
.DELETE_ON_ERROR:


#==========================================================
#	 Special flags
#==========================================================

# Compiler flags
CFLAGS := -std=c99 -ffreestanding -Wall -Wextra -Werror -pedantic -nostdlib -m32
CXXFLAGS := -std=c++17 -ffreestanding -Wall -Wextra -Werror -pedantic -nostdlib -m32
ASFLAGS :=

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L. 
# Libraries (-lfoo) should be added to the LDLIBS variable instead. 
LDFLAGS :=
# Library flags or names given to compilers when they are supposed to invoke the linker, ‘ld’. 
# Non-library linker flags, such as -L, should go in the LDFLAGS variable
LDLIBS :=


#==========================================================
#	 Default rule
#==========================================================

.PHONY: all
all: run


#==========================================================
#	 Regular build
#==========================================================

# Build the OS
$(OS_IMAGE): $(BIN_DIR)/bootsect.bin $(KERNEL_BIN)
	cat $^ > $@

# Build  the  kernel  binary
$(KERNEL_BIN): $(OBJECTS) $(BUILD_DIR)/boot/boot.o
	mkdir -p $(@D)
	gcc -m32 -T boot/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^

$(BUILD_DIR)/boot/boot.o: boot/boot.s
	mkdir -p $(@D)
	as -32 $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	echo "    \e[96m[object]\e[0m $@"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	echo "    \e[96m[object]\e[0m $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<


#==========================================================
#	 Run
#==========================================================

.PHONY: run
run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $<


#==========================================================
#	 Clean
#==========================================================

.PHONY: clean
clean:
	rm -rf build
	rm -rf bin







