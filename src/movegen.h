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

/*
 * All legal moves for the side to move: every pseudo-legal move from
 * the four generators above, filtered by actually playing each one
 * (via Position::makeMove/unmakeMove) and keeping it only if the
 * mover's own king isn't left in check. See
 * https://www.chessprogramming.org/Legal_Move.
 */
[[nodiscard]] MoveList generateLegalMoves(const Position& position);

} // namespace MoveGen

#endif
