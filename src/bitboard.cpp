#include "bitboard.h"
#include "types.h" //U64

/*
 * Count number of set bits in bitboard.
 * Consecutively reset LS1B in a loop body and counting 
 * loop cycles until the bitset becomes empty.
 * Method published by Peter Wegner.
 */
int popcount(U64 bitboard)
{
    int count { 0 };
    while(bitboard)
    {
        bitboard &= bitboard - 1;
        ++count;
    }
    return count;
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

/*
 * Take in a LERFSquare, and return a U64
 * bitboard with the relevant square bit set.
 */
U64 squareToBitboard(int sq)
{
    return (1ULL << sq);
}