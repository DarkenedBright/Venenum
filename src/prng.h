#ifndef PRNG_H
#define PRNG_H

#include "types.h" // U64

/* 
 * Use xorshift pseudorandom number generator
 * Credit: Sebastiano Vigna, Universita degli Studi di Milano, Italy
 * https://vigna.di.unimi.it/ftp/papers/xorshift.pdf
 */
class PRNG
{
    private:
        U64 xorShiftNum {};
    public:
        explicit PRNG(U64 seed) : xorShiftNum{ seed }{}
        U64 xorShiftRand() {
            xorShiftNum ^= xorShiftNum >> 12;
            xorShiftNum ^= xorShiftNum << 25;
            xorShiftNum ^= xorShiftNum >> 27;
            return xorShiftNum * 2685821657736338717;
        }
};
#endif