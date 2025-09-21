export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
INCLUDE_DIR = include
ARTIFACTS = ../target/artifacts/libc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)

SRC_FILES := $(shell find $(SRC) -name "*.c")

OBJ_FILES := $(patsubst $(SRC)/%.c,$(ARTIFACTS)/%.o,$(SRC_FILES))

$(ARTIFACTS)/%.o: $(SRC)/%.c | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

all: $(OBJ_FILES)

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
