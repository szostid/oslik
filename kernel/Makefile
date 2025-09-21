export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
INCLUDE_DIR = include ../libc/include
ARTIFACTS = ../target/artifacts

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)
LD_SCRIPT = $(SRC)/linker.ld
OBJ = $(ARTIFACTS)/boot.o $(ARTIFACTS)/kernel.o $(ARTIFACTS)/tty.o

all: $(OBJ)

$(ARTIFACTS)/boot.o: $(SRC)/boot.S | $(ARTIFACTS)
	i686-elf-as $< -o $@

$(ARTIFACTS)/kernel.o: $(SRC)/kernel.c | $(ARTIFACTS)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS)/tty.o: $(SRC)/tty.c | $(ARTIFACTS)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
