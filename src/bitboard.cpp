#include "bitboard.h"
#include "types.h" //U64

#include <bit> // std::popcount

/*
 * Count number of set bits in bitboard.
 * Delegates to the C++20 standard library, which typically
 * lowers to a single hardware popcount instruction.
 */
int popcount(U64 bitboard)
{
    return std::popcount(bitboard);
}

/*
 * Take in a bitboard and square, and
 * set the square bit to 0.
 */
U64 resetBit(U64 bitboard, int sq)
{
    bitboard &= ~squareToBitboard(sq);
    return bitboard;
}

/*
 * Take in a bitboard and square, and
 * set the square bit to 1.
 */
U64 setBit(U64 bitboard, int sq)
{
    bitboard |= squareToBitboard(sq);
    return bitboard;
}