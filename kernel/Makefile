export PATH := $(HOME)/Desktop/gcc-cross/bin:$(PATH)

SRC = src
ARTIFACTS = ../target/artifacts/kernel

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include -I ../libc/include -D SERIAL_WRITE_TTY

C_FILES := $(shell find $(SRC) -name "*.c")
ASM_FILES := $(shell find $(SRC) -name "*.S")

C_OBJS := $(patsubst $(SRC)/%.c,$(ARTIFACTS)/%.c.o,$(C_FILES))
ASM_OBJS := $(patsubst $(SRC)/%.S,$(ARTIFACTS)/%.S.o,$(ASM_FILES))

OBJ_FILES := $(C_OBJS) $(ASM_OBJS)

$(ARTIFACTS)/%.c.o: $(SRC)/%.c | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

$(ARTIFACTS)/%.S.o: $(SRC)/%.S | $(ARTIFACTS)
	@mkdir -p $(dir $@)
	i686-elf-as $< -o $@

all: $(OBJ_FILES)

$(ARTIFACTS):
	mkdir -p $(ARTIFACTS)
