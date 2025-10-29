#include <stdint.h>

static uint32_t seed;

uint64_t rdtsc(void)
{
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

uint32_t rand()
{
    seed = seed * 1664525 + 1013904223;
    return seed;
}

void srand(uint32_t new_seed)
{
    seed = new_seed;
}
