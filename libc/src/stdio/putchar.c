#include <stdio.h>

#if defined(__is_libk)
#include <tty.h>
#endif

int putchar(int ic)
{
#if defined(__is_libk)
    char c = (char)ic;
    terminal_write(&c, 1);
#else
#error "Cannot build libc for non-kernel targets yet"
#endif
    return ic;
}