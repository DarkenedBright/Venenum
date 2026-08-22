#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h" // U64

[[nodiscard]] int popcount(U64 bitboard);
[[nodiscard]] U64 resetBit(U64 bitboard, int sq);
[[nodiscard]] U64 setBit(U64 bitboard, int sq);

/*
 * Take in a LERFSquare, and return a U64
 * bitboard with the relevant square bit set.
 * Marked constexpr so it can be used from consteval
 * compile-time attack-table generation (see attack.h).
 */
[[nodiscard]] constexpr U64 squareToBitboard(int sq)
{
    return (1ULL << sq);
}

#endif