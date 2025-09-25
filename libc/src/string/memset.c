#include <string.h>

void *memset(void *dest, int ch, size_t count)
{
    const unsigned char byte = (unsigned char)ch;
    unsigned char *dest_bytes = (unsigned char *)dest;

    for (size_t i = 0; i < count; i++)
    {
        dest_bytes[i] = byte;
    }

    return dest;
}