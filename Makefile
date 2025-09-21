ARTIFACTS = target/artifacts
BIN = $(ARTIFACTS)/myos.bin
ISO_DIR = target/isoout
LD_SCRIPT = kernel/src/linker.ld

.PHONY: all clean iso run

all: $(BIN)

$(BIN):
	$(MAKE) -C libc
	$(MAKE) -C kernel
	i686-elf-gcc -ffreestanding -O2 -nostdlib -lgcc \
        $(ARTIFACTS)/boot.o $(ARTIFACTS)/kernel.o $(ARTIFACTS)/tty.o $(ARTIFACTS)/libc.o \
		-T $(LD_SCRIPT) -o $(BIN)
	grub-file --is-x86-multiboot $(BIN)

iso: $(BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BIN) $(ISO_DIR)/boot/myos.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o target/myos.iso $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom target/myos.iso

clean:
	rm -rf target