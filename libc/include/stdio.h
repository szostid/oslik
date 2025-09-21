#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#define EOF (-1)

int printf(const char *__restrict fmt, ...);
int vprintf(const char *__restrict fmt, va_list parameters);

int putchar(int);
int puts(const char *);

#endif