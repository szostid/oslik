#ifndef PANIC_H
#define PANIC_H

/// @brief Halts the execution of the kernel with the provided message
/// @param format The format string, formatted in the same way as `printf`.
__attribute__((__noreturn__)) void kpanic(const char *__restrict fmt, ...);

/// @brief Starts the logging of a kernel panic. Clears the terminal,
/// changes the background color, and logs a generic `KERNEL PANIC` at the top.
/// You should use `end_kpanic` later to actually finish the panicking process.
///
/// You can log with `printf` to provide more user info later.
///
/// @attention If you want a simple single formatted message, use `kpanic`.
void start_kpanic();

/// @brief Ends the logging of a kernel panic.
///
/// Call this after `start_kpanic` and after you log all your data to the
/// console.
///
/// @attention If you want a simple single formatted message, use `kpanic`.
__attribute__((__noreturn__)) void end_kpanic();

#endif