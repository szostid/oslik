#include <string.h>

void *memcpy_f(void *__restrict dest, const void *__restrict src, size_t count)
{
    unsigned char *dest_bytes = (unsigned char *)dest;
    const unsigned char *src_bytes = (const unsigned char *)src;

    for (size_t i = 0; i < count; i++)
    {
        dest_bytes[i] = src_bytes[i];
    }

    return dest;
}