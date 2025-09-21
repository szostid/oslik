export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
INCLUDE_DIR = include
TARGET = target
ARTIFACTS = $(TARGET)/artifacts
ISO_DIR = $(TARGET)/isoout

BIN = $(ARTIFACTS)/myos.bin

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)

LD_SCRIPT = $(SRC)/linker.ld

all: $(BIN)

$(ARTIFACTS)/boot.o: $(SRC)/boot.S | $(ARTIFACTS)
	i686-elf-as $< -o $@

$(ARTIFACTS)/kernel.o: $(SRC)/kernel.c | $(ARTIFACTS)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS)/tty.o: $(SRC)/tty.c | $(ARTIFACTS)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(BIN): $(ARTIFACTS)/boot.o $(ARTIFACTS)/kernel.o $(ARTIFACTS)/tty.o
	i686-elf-gcc -ffreestanding -O2 -nostdlib -lgcc -T $(LD_SCRIPT) $^ -o $(BIN)
	grub-file --is-x86-multiboot $(BIN)

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)

.PHONY: clean
clean:
	rm -rf $(TARGET)

.PHONY: iso
iso: $(BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BIN) $(ISO_DIR)/boot/myos.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(TARGET)/myos.iso $(ISO_DIR)

.PHONY: run
run: iso
	qemu-system-i386 -cdrom $(TARGET)/myos.iso
