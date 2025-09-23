export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

# Define tools and paths once at the top level
PREFIX = i686-elf
LD = $(PREFIX)-gcc
GRUB_FILE = $(PREFIX)-grub-file
GRUB_MKRESCUE = $(PREFIX)-grub-mkrescue

# Project Structure
ARTIFACTS = target/artifacts
BIN = $(ARTIFACTS)/myos.bin
ISO_DIR = target/isoout
LD_SCRIPT = kernel/linker.ld

# Linker Flags
LDFLAGS = -T $(LD_SCRIPT) -ffreestanding -O2 -nostdlib -lgcc

# Phony targets do not represent files and will always run their recipes.
.PHONY: all clean iso run build_kernel build_libc

all: $(BIN)

$(BIN): build_kernel build_libc $(LD_SCRIPT)
	@echo "LD $@"
# Find all .o files *after* sub-makes have run and pass them to the linker.
	$(LD) -o $@ $(LDFLAGS) $(shell find $(ARTIFACTS) -name "*.o")
	i686-elf-grub-file --is-x86-multiboot $@

build_kernel:
	$(MAKE) -C kernel

build_libc:
	$(MAKE) -C libc

iso: $(BIN)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(BIN) $(ISO_DIR)/boot/myos.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	i686-elf-grub-mkrescue -o target/myos.iso $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom target/myos.iso -serial file:kernel.log

clean:
	@echo "Cleaning up project files..."
	rm -rf target