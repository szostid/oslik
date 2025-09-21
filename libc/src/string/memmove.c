#include <string.h>

void *memmove_f(void *dest, const void *src, size_t count)
{
    unsigned char *dest_bytes = (unsigned char *)dest;
    const unsigned char *src_bytes = (const unsigned char *)src;

    // The memory may be overlapping, so we need to ensure that we aren't
    // copying into the memory from `src_bytes` that we haven't written to
    // `dest` yet.

    if (dest_bytes < src_bytes)
    {
        for (size_t i = 0; i < count; i++)
            dest_bytes[i] = src_bytes[i];
    }
    else
    {
        for (size_t i = count; i != 0; i--)
            dest_bytes[i - 1] = src_bytes[i - 1];
    }

    return dest;
}