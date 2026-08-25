#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "move.h" // Move, MoveList

#include <cstdint> // std::uint64_t
#include <optional> // std::optional
#include <string_view> // std::string_view

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

/*
 * Count the leaf nodes reachable from position after exactly depth
 * plies of legal moves. The standard move-generator correctness test:
 * known node counts for well-studied positions (starting position,
 * Kiwipete) catch subtle move-generation bugs that unit tests on
 * individual pieces miss. See
 * https://www.chessprogramming.org/Perft.
 */
[[nodiscard]] std::uint64_t perft(const Position& position, int depth);

/*
 * Parse a UCI move string (e.g. "e2e4", "e7e8q") against position's
 * legal moves, returning the matching Move or std::nullopt if the
 * string is malformed or doesn't name a legal move. Matches by
 * comparing against each legal move's own toUCIString() rather than
 * parsing squares independently, so a malformed or illegal string
 * can never accidentally decode into a Move. See
 * https://www.chessprogramming.org/UCI.
 */
[[nodiscard]] std::optional<Move> parseUCIMove(const Position& position, std::string_view moveString);

} // namespace MoveGen

#endif
