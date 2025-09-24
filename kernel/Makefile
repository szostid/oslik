export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
ARTIFACTS = ../target/artifacts/kernel

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include -I ../libc/include -D SERIAL_WRITE_TTY

SRC_FILES := $(shell find $(SRC) -name "*.c")

OBJ_FILES := $(patsubst $(SRC)/%.c,$(ARTIFACTS)/%.c.o,$(SRC_FILES)) \
             $(ARTIFACTS)/gdt.S.o \
			 $(ARTIFACTS)/idt.S.o \
             $(ARTIFACTS)/boot.S.o

$(ARTIFACTS)/%.c.o: $(SRC)/%.c | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS)/%.S.o: $(SRC)/%.S | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-as $< -o $@

all: $(OBJ_FILES)

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
