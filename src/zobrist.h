#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "board.h"

#include <cstdint> // uint_64

constexpr uint64_t ZOBRIST_SEED { 0xFD2D8157399E58D4 };

struct ZobristKeys
{
    uint64_t pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
    uint64_t sideToMoveKey;
    uint64_t castlingRightKeys[NUM_CASTLE_STATES];
    uint64_t enPassantFileKeys[NUM_FILES];
};

uint64_t getPositionHash(Board& position);

#endif