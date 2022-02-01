#include "board.h"
#include "zobrist.h"

#include <cstdint> // uint_64

/* 
 * Use xorshift pseudorandom number generator
 * Credit: Sebastiano Vigna, Universita degli Studi di Milano, Italy
 * https://vigna.di.unimi.it/ftp/papers/xorshift.pdf
 */

uint64_t xorShiftNum { ZOBRIST_SEED };

uint64_t xorShiftRand() {
    xorShiftNum ^= xorShiftNum >> 12;
    xorShiftNum ^= xorShiftNum << 25;
    xorShiftNum ^= xorShiftNum >> 27;
    return xorShiftNum * 2685821657736338717;
}

/* 
 * Use zobrist hashing
 * Credit: Albert L. Zobrist, The University of Wisconsin
 * https://research.cs.wisc.edu/techreports/1970/TR88.pdf
 */

ZobristKeys initZobristKeys()
{
    ZobristKeys keys {};

    // uint64_t pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        for(int piece { EMPTY }; piece < NUM_PIECES; ++piece)
        {
            keys.pieceSquareKeys[sq][piece] = xorShiftRand();
        }
    }

    // uint64_t sideToMoveKey;
    keys.sideToMoveKey = xorShiftRand();

    // uint64_t castlingRightKeys[NUM_CASTLE_STATES];
    for(int castle { 0 }; castle < NUM_CASTLE_STATES; ++castle)
    {
        keys.castlingRightKeys[castle] = xorShiftRand();
    }

    // uint64_t enPassantFileKeys[NUM_FILES];
    for(int file { FILE_A }; file < NUM_FILES; ++file)
    {
        keys.enPassantFileKeys[file] = xorShiftRand();
    }

    return keys;
}

const ZobristKeys zobristKeys { initZobristKeys() };

uint64_t getPositionHash(Board& position)
{
    uint64_t hash { 0 };

    //Handle piece square keys
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        Piece piece = position.piecesOnBoard[sq];
        hash ^= zobristKeys.pieceSquareKeys[sq][piece];
    }

    //Handle side to move
    if(position.sideToMove == BLACK)
    {
        hash ^= zobristKeys.sideToMoveKey;
    }

    //Handle castling rights
    hash ^= zobristKeys.castlingRightKeys[position.castlingRights];

    //Handle enPassant File
    if(position.enPassantSquare != NO_SQ)
    {
        int file = position.enPassantSquare % 8;
        hash ^= zobristKeys.enPassantFileKeys[file];
    }

    return hash;
}
