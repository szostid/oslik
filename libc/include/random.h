#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

uint32_t rand(void);
uint64_t rdtsc(void);
void srand(uint32_t new_seed);

#endif
