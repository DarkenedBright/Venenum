#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h" // U64 

int popcount(U64 bitboard);
U64 resetBit(U64 bitboard, int sq);
U64 setBit(U64 bitboard, int sq);
U64 squareToBitboard(int sq);

#endif