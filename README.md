# Oslik
A very basic operating system created by me to learn C.

This system is implemented based on the [osdev guide](https://wiki.osdev.org/Tutorials).

## Preview

https://github.com/user-attachments/assets/a1e515e7-7491-4e30-8e28-c241de16f2dc

https://github.com/user-attachments/assets/97a76a5f-6929-4b0f-8228-2f5476d80f94

#### Features
As said previously, the operating system is very basic so there's not much that can be done. These are some of the things that are working:

- A tetris game
- A pong game
- TTY commands
- Interrupts and input handling

And these are some of the things that are NOT working (and I'd like to implement them one day):

- Paging
- User-space
- ELF loading

## Installation
#### Prerequisites
- A [gcc cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) for `i686-elf` (you will probably have to change the path in all `Makefile`s - it's currently `$HOME/Desktop/gcc-cross/bin`)
- Grub tools `grub-mkrescue` and `grub-file` for `i686-elf`
- A virtual machine that supports `i386` (QEMU or Bochs), if you plan to actually run the OS

#### Running
Use `make run` to open the OS in QEMU and `make run_bochs` to run it in bochs, or `make iso` to just build the ISO
