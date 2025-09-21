export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

ARTIFACTS = target/artifacts
BIN = $(ARTIFACTS)/myos.bin
ISO_DIR = target/isoout
LD_SCRIPT = kernel/linker.ld

OBJ_FILES := $(shell find $(ARTIFACTS) -name "*.o")

.PHONY: all clean iso run

all: $(BIN)

$(BIN):
	$(MAKE) -C libc
	$(MAKE) -C kernel
	i686-elf-gcc -T $(LD_SCRIPT) -o $@ -ffreestanding -O2 -nostdlib $(OBJ_FILES) -lgcc
	i686-elf-grub-file --is-x86-multiboot $@

.PHONY: iso
iso: $(BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BIN) $(ISO_DIR)/boot/myos.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	i686-elf-grub-mkrescue -o target/myos.iso $(ISO_DIR)

.PHONY: run
run: iso
	qemu-system-i386 -cdrom target/myos.iso

clean:
	rm -rf target