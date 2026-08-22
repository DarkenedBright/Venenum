#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h" // U64

[[nodiscard]] int popcount(U64 bitboard);
[[nodiscard]] U64 resetBit(U64 bitboard, int sq);
[[nodiscard]] U64 setBit(U64 bitboard, int sq);
[[nodiscard]] U64 squareToBitboard(int sq);

#endif