KERNEL_BIN := target/x86_64-aurora-os/debug/bootimage-aurora-os.bin

.PHONY: all
all: help

## Development:

b: build  	## alias: build
build:  	## Build the kernel binary
	cargo bootimage -Zbuild-std=core,compiler_builtins -Zbuild-std-features=compiler-builtins-mem

r: run  	## alias: run
run: build  ## Run the kernel binary in qemu
	qemu-system-x86_64 -drive format=raw,file=target/x86_64-aurora-os/debug/bootimage-aurora-os.bin

c: clean  	## alias: clean
clean:  	## Clean the build
	cargo clean

t: test  	## alias: test
test:  		## Run the tests
	cargo t --target x86_64-apple-darwin

## Help:
.SILENT: help
help: ## Show this help.
	# Self generating help
	# Inspired from https://gist.github.com/thomaspoignant/5b72d579bd5f311904d973652180c705#file-makefile-L89
	echo 'Usage:'
	echo '  make [target]...'
	echo ''
	echo 'Targets:'
	awk 'BEGIN {FS = ":.*?## "} { \
		if (/^[a-zA-Z_-]+:.*?##.*$$/) {printf "        %-20s%s\n", $$1, $$2} \
		else if (/^## .*$$/) {printf "\n    %s\n", substr($$1,4)} \
		}' $(MAKEFILE_LIST)
	echo ''