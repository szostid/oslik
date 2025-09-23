#ifndef PANIC_H
#define PANIC_H

/// @brief Halts the execution of the kernel with the provided message
/// @param format The format string, formatted in the same way as `printf`.
__attribute__((__noreturn__)) void kpanic(const char *__restrict fmt, ...);

#endif