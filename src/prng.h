#ifndef PRNG_H
#define PRNG_H

#include <cstdint> //std::uint64_t

/* 
 * Use xorshift pseudorandom number generator
 * Credit: Sebastiano Vigna, Universita degli Studi di Milano, Italy
 * https://vigna.di.unimi.it/ftp/papers/xorshift.pdf
 */
class PRNG
{
    private:
        std::uint64_t xorShiftNum {};
    public:
        PRNG(std::uint64_t seed) : xorShiftNum{ seed }{}
        std::uint64_t xorShiftRand() {
            xorShiftNum ^= xorShiftNum >> 12;
            xorShiftNum ^= xorShiftNum << 25;
            xorShiftNum ^= xorShiftNum >> 27;
            return xorShiftNum * 2685821657736338717;
        }
};
#endif