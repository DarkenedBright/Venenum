#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "move.h" // MoveList

class Position;

namespace MoveGen
{

void generateKnightMoves(const Position& position, MoveList& moves);
void generateKingMoves(const Position& position, MoveList& moves);
void generateSlidingMoves(const Position& position, MoveList& moves);
void generatePawnMoves(const Position& position, MoveList& moves);

} // namespace MoveGen

#endif
