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
SRC_DIRS := boot kernel libcpp
INC_DIRS := $(SRC_DIRS)
# INC_FLAGS := $(addprefix -I,$(INC_DIRS))
KERNEL_BIN := $(BIN_DIR)/boot/kernel.bin
GRUB_CFG := $(BIN_DIR)/boot/grub/grub.cfg
OS_IMAGE := myos.iso

# C specifics
C_SOURCES := $(shell find $(SRC_DIRS) -mindepth 1 -type f -name *.c)
C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(C_SOURCES)))

# C++ specifics
CXX_SOURCES := $(shell find $(SRC_DIRS) -mindepth 1 -type f -name *.cpp)
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(CXX_SOURCES)))

# ASM specifics
AS_SOURCES := $(shell find $(SRC_DIRS) -mindepth 1 -type f -name *.s)
AS_OBJECTS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(AS_SOURCES)))

# Aggregation of all the objects
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS) $(AS_OBJECTS)

# Delete targets in case of error
.DELETE_ON_ERROR:

# Disable invocation echoing
# .SILENT:

# Disable implicit rules (faster exec + easier Makefile debugging)
MAKEFLAGS += --no-builtin-rules


#==========================================================
#	 Special flags
#==========================================================

# Compiler flags
C_CXX_FLAGS := $(shell cat compile_flags.txt | tr '\n' ' ') -m32
CFLAGS := $(C_CXX_FLAGS) -std=c99 -g
CXXFLAGS := $(C_CXX_FLAGS) -std=c++17 -fno-exceptions -fno-rtti -g
ASFLAGS := -32 -g

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L. 
# Libraries (-lfoo) should be added to the LDLIBS variable instead. 
# LDFLAGS :=
# Library flags or names given to compilers when they are supposed to invoke the linker, ‘ld’. 
# Non-library linker flags, such as -L, should go in the LDFLAGS variable
# LDLIBS :=


#==========================================================
#	 Utils
#==========================================================

BLACK := 30
RED := 31
GREEN := 32
YELLOW := 33
BLUE := 34
MAGENTA := 35
CYAN := 36
WHITE := 37
DARK_GRAY := 90
LRED := 91
LGREEN := 92
LYELLOW := 93
LBLUE := 94
LMAGENTA := 95
LCYAN := 96
WHITE := 97

MAX_STEP_SIZE := 8
define log_step
	printf "    \e[%sm%-$(MAX_STEP_SIZE)s\e[0m %s\n" "$(strip $(1))" "[$(strip $(2))]" "$(strip $(3))"
endef


#==========================================================
#	 Default rule
#==========================================================

.PHONY: all
all: run


#==========================================================
#	 Regular build
#==========================================================

# Convenience rule
.PHONY: build-iso
build-iso: $(OS_IMAGE)

$(OS_IMAGE): $(KERNEL_BIN)
	@$(call log_step, $(YELLOW), image, $@)
	@mkdir -p `dirname $(GRUB_CFG)`
	@rm -rf $(GRUB_CFG)
	@echo "menuentry \"$(OS_IMAGE:.iso=)\" { 		\n\
		multiboot $(KERNEL_BIN:$(BIN_DIR)/%=/%)	\n\
	}" > $(GRUB_CFG)
	grub-mkrescue -o $(OS_IMAGE) $(BIN_DIR)

# Convenience rule
.PHONY: build
build: $(KERNEL_BIN)

$(KERNEL_BIN): $(OBJECTS)
	@$(call log_step, $(LMAGENTA), binary, $@)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -T boot/linker.ld -o $@ $^
	grub-file --is-x86-multiboot $@

#$(BUILD_DIR)/boot/boot.o: boot/boot.s
#	$(call log_step, $(LCYAN), object, $<)
#	mkdir -p $(@D)
#	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	@$(call log_step, $(LCYAN), object, $<)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(call log_step, $(LCYAN), object, $<)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.s.o: %.s
	@$(call log_step, $(LCYAN), object, $<)
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@


#==========================================================
#	 Run
#==========================================================

.PHONY: run
run: $(KERNEL_BIN)
	@$(call log_step, $(LGREEN), run, $<)
	qemu-system-i386 -kernel $<

.PHONY: run-iso
run-iso: $(OS_IMAGE)
	@$(call log_step, $(LGREEN), run, $<)
	qemu-system-i386 -cdrom $<


#==========================================================
#	 Clean
#==========================================================

define clean_file
	@$(call log_step, $(LRED), delete, $(1))
	rm -rf $(1)
endef

.PHONY: clean
clean:
	$(call clean_file, build)
	$(call clean_file, bin)
	$(call clean_file, $(OS_IMAGE))







