export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
ARTIFACTS = ../target/artifacts/kernel

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include -I ../libc/include -D SERIAL_WRITE_TTY

SRC_FILES := $(shell find $(SRC) -name "*.c")

OBJ_FILES := $(patsubst $(SRC)/%.c,$(ARTIFACTS)/%.o,$(SRC_FILES)) \
             $(ARTIFACTS)/boot.o

$(ARTIFACTS)/%.o: $(SRC)/%.c | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS)/%.o: $(SRC)/%.S | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-as $< -o $@

all: $(OBJ_FILES)

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
