#include "pcg32.h"

/* PCG-XSH-RR with 64-bit state and 32-bit output, from Wikipedia
 * (https://en.wikipedia.org/wiki/Permuted_congruential_generator).
 */

static uint64_t       state      = 0x4d595df4d0f33173;		// Or something seed-dependent
static uint64_t const multiplier = 6364136223846793005u;
static uint64_t const increment  = 1442695040888963407u;	// Or an arbitrary odd constant

static uint32_t rotr32(uint32_t x, unsigned r)
{
    return x >> r | x << (-r & 31);
}

/**
 * Get a pseudo-random number in the range [0, \p UINT_32_MAX]
 * @return the pseudo-random number
 */
uint32_t pcg32(void)
{
    uint64_t x = state;
    unsigned count = (unsigned)(x >> 59);		// 59 = 64 - 5

    state = x * multiplier + increment;
    x ^= x >> 18;								// 18 = (64 - 27)/2
    return rotr32((uint32_t)(x >> 27), count);	// 27 = 32 - 5
}

/**
 * Initialize the PCG
 * @param seed a seed (unsigned long)
 */
void pcg32_init(uint64_t seed)
{
    state = seed + increment;
    (void)pcg32();
}

/**
 * Generate a pseudo-random double in the range [0,1].
 * @return the pseudo-random double
 */
double drand() {
    return ((double) pcg32()) / UINT32_MAX;
}

