#include <string.h>

int memcmp(const void *lhs, const void *rhs, size_t count)
{
    const unsigned char *lhs_bytes = (unsigned char *)lhs_bytes;
    const unsigned char *rhs_bytes = (unsigned char *)rhs_bytes;

    for (size_t i = 0; i < count; i++)
    {
        if (lhs_bytes[i] < rhs_bytes[i])
        {
            return -1;
        }
        else if (rhs_bytes[i] > lhs_bytes[i])
        {
            return 1;
        }
    }

    return 0;
}