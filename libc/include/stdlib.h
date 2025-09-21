#ifndef STDLIB_H
#define STDLIB_H

/// @brief Halts the execution of the program, never returning.
__attribute__((__noreturn__)) void abort(void);

/// @brief Halts the execution of the program with the provided message, never
/// returning.
/// @param format The format string, formatted in the same way as `printf`.
__attribute__((__noreturn__)) void panic(const char *__restrict fmt, ...);

#endif