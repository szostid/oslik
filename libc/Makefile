export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
INCLUDE_DIR = include
ARTIFACTS = ../target/artifacts

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)

OBJ = $(ARTIFACTS)/libc.o

all: $(OBJ)

$(ARTIFACTS)/libc.o: $(SRC)/test.c | $(ARTIFACTS)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
