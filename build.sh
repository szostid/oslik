#!/usr/bin/env bash
set -e  # exit on any error
set -u  # error on undefined variables

# Path to your cross-compiler
export PATH="$HOME/Desktop/gcc-cross/bin:$PATH"

# Directories
BUILD_DIR="./target"
ARTIFACTS_DIR="$BUILD_DIR/artifacts"
ISO_DIR="$BUILD_DIR/isoout"
SRC_DIR="./src"
INCLUDE_DIR="./include"

# Clean previous build
rm -rf "$BUILD_DIR"

# Create necessary directories
mkdir -p "$ARTIFACTS_DIR" "$ISO_DIR/boot/grub"

# Assemble and compile
i686-elf-as "$SRC_DIR/boot.S" -o "$ARTIFACTS_DIR/boot.o"
i686-elf-gcc -I "$INCLUDE_DIR"  -c "$SRC_DIR/kernel.c" -o "$ARTIFACTS_DIR/kernel.o" -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -I "$INCLUDE_DIR"  -c "$SRC_DIR/tty.c" -o "$ARTIFACTS_DIR/tty.o" -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i686-elf-gcc -I "$INCLUDE_DIR" -T "$SRC_DIR/linker.ld" -o "$ARTIFACTS_DIR/myos.bin" -ffreestanding -O2 -nostdlib "$ARTIFACTS_DIR/boot.o" "$ARTIFACTS_DIR/kernel.o" "$ARTIFACTS_DIR/tty.o" -lgcc

# Verify multiboot
grub-file --is-x86-multiboot "$ARTIFACTS_DIR/myos.bin"

# Prepare ISO directory
cp "$ARTIFACTS_DIR/myos.bin" "$ISO_DIR/boot/myos.bin"
cp grub.cfg "$ISO_DIR/boot/grub/grub.cfg"

# Build ISO
grub-mkrescue -o "$BUILD_DIR/myos.iso" "$ISO_DIR"

# Launch in QEMU
qemu-system-i386 -cdrom "$BUILD_DIR/myos.iso"
