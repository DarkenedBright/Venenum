#include "movegen.h"
#include "attack.h" // Attack::knightAttacks, Attack::kingAttacks, Attack::bishopAttacks, Attack::rookAttacks, Attack::queenAttacks
#include "bitboard.h" // getLSBIndex, resetBit, squareToBitboard
#include "move.h" // Move, MoveFlag, MoveList
#include "position.h" // Position
#include "types.h" // LERFSquare, Piece, Side, U64

namespace
{

/*
 * Turn an attack bitboard from square 'from' into Move objects
 * appended to moves. Destinations on ownPieces are excluded (a piece
 * can't capture its own side); destinations on enemyPieces become
 * CAPTURE moves, everything else a QUIET move.
 */
void serializeMoves(LERFSquare from, U64 attacks, U64 ownPieces, U64 enemyPieces, MoveList& moves)
{
    U64 targets { attacks & ~ownPieces };
    while(targets)
    {
        int toSq { getLSBIndex(targets) };
        MoveFlag flag { (enemyPieces & squareToBitboard(toSq)) ? MoveFlag::CAPTURE : MoveFlag::QUIET };
        moves.emplace_back(from, static_cast<LERFSquare>(toSq), flag);
        targets = resetBit(targets, toSq);
    }
}

using SlidingAttackFn = U64 (*)(LERFSquare, U64);

/*
 * Generate moves for every piece on pieceBitboard using a sliding
 * attack function -- rook, bishop, and queen attacks all share this
 * shape: they depend on the square and the board's occupancy.
 */
void generateSlidingPieceMoves(U64 pieceBitboard, SlidingAttackFn attackFn, U64 occupancy, U64 ownPieces, U64 enemyPieces, MoveList& moves)
{
    while(pieceBitboard)
    {
        int fromSq { getLSBIndex(pieceBitboard) };
        LERFSquare from { static_cast<LERFSquare>(fromSq) };
        serializeMoves(from, attackFn(from, occupancy), ownPieces, enemyPieces, moves);
        pieceBitboard = resetBit(pieceBitboard, fromSq);
    }
}

} // namespace

void MoveGen::generateKnightMoves(const Position& position, MoveList& moves)
{
    Side side { position.getSideToMove() };
    Piece knightPiece { side == Side::WHITE ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT };
    U64 ownPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::WHITE_ALL : Piece::BLACK_ALL) };
    U64 enemyPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::BLACK_ALL : Piece::WHITE_ALL) };

    U64 knights { position.getPieceBitboard(knightPiece) };
    while(knights)
    {
        int fromSq { getLSBIndex(knights) };
        LERFSquare from { static_cast<LERFSquare>(fromSq) };
        serializeMoves(from, Attack::knightAttacks(from), ownPieces, enemyPieces, moves);
        knights = resetBit(knights, fromSq);
    }
}

void MoveGen::generateKingMoves(const Position& position, MoveList& moves)
{
    Side side { position.getSideToMove() };
    Piece kingPiece { side == Side::WHITE ? Piece::WHITE_KING : Piece::BLACK_KING };
    U64 ownPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::WHITE_ALL : Piece::BLACK_ALL) };
    U64 enemyPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::BLACK_ALL : Piece::WHITE_ALL) };

    U64 kingBitboard { position.getPieceBitboard(kingPiece) };
    if(kingBitboard)
    {
        LERFSquare from { static_cast<LERFSquare>(getLSBIndex(kingBitboard)) };
        serializeMoves(from, Attack::kingAttacks(from), ownPieces, enemyPieces, moves);
    }
}

void MoveGen::generateSlidingMoves(const Position& position, MoveList& moves)
{
    Side side { position.getSideToMove() };
    U64 ownPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::WHITE_ALL : Piece::BLACK_ALL) };
    U64 enemyPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::BLACK_ALL : Piece::WHITE_ALL) };
    U64 occupancy { position.getPieceBitboard(Piece::ALL_PIECES) };

    Piece bishopPiece { side == Side::WHITE ? Piece::WHITE_BISHOP : Piece::BLACK_BISHOP };
    Piece rookPiece { side == Side::WHITE ? Piece::WHITE_ROOK : Piece::BLACK_ROOK };
    Piece queenPiece { side == Side::WHITE ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN };

    generateSlidingPieceMoves(position.getPieceBitboard(bishopPiece), Attack::bishopAttacks, occupancy, ownPieces, enemyPieces, moves);
    generateSlidingPieceMoves(position.getPieceBitboard(rookPiece), Attack::rookAttacks, occupancy, ownPieces, enemyPieces, moves);
    generateSlidingPieceMoves(position.getPieceBitboard(queenPiece), Attack::queenAttacks, occupancy, ownPieces, enemyPieces, moves);
}
