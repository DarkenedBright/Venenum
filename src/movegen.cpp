#include "movegen.h"
#include "attack.h" // Attack::knightAttacks, Attack::kingAttacks, Attack::bishopAttacks, Attack::rookAttacks, Attack::queenAttacks, Attack::pawnAttacks
#include "bitboard.h" // getLSBIndex, resetBit, squareToBitboard
#include "move.h" // Move, MoveFlag, MoveList
#include "position.h" // Position
#include "types.h" // LERFSquare, Piece, Rank, Side, U64

#include <utility> // std::to_underlying

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

/*
 * Whether sq is on the rank a side's pawn promotes from, i.e. the
 * last rank before the edge of the board in that side's direction of
 * travel.
 */
[[nodiscard]] bool isPawnPromotionRank(Side side, int sq)
{
    int rank { sq / 8 };
    return side == Side::WHITE ? rank == std::to_underlying(Rank::RANK_8) : rank == std::to_underlying(Rank::RANK_1);
}

/*
 * Whether sq is on the rank a side's pawns start the game on, the
 * only rank a double push can originate from.
 */
[[nodiscard]] bool isPawnStartingRank(Side side, int sq)
{
    int rank { sq / 8 };
    return side == Side::WHITE ? rank == std::to_underlying(Rank::RANK_2) : rank == std::to_underlying(Rank::RANK_7);
}

/*
 * Append the move(s) a pawn reaching 'to' produces: a single QUIET
 * or CAPTURE move normally, or all four promotion (-capture) flags
 * at once when 'to' is on the far rank.
 */
void addPawnMove(LERFSquare from, LERFSquare to, bool isCaptureMove, bool isPromotionMove, MoveList& moves)
{
    if(!isPromotionMove)
    {
        moves.emplace_back(from, to, isCaptureMove ? MoveFlag::CAPTURE : MoveFlag::QUIET);
        return;
    }

    if(isCaptureMove)
    {
        moves.emplace_back(from, to, MoveFlag::KNIGHT_PROMO_CAPTURE);
        moves.emplace_back(from, to, MoveFlag::BISHOP_PROMO_CAPTURE);
        moves.emplace_back(from, to, MoveFlag::ROOK_PROMO_CAPTURE);
        moves.emplace_back(from, to, MoveFlag::QUEEN_PROMO_CAPTURE);
    }
    else
    {
        moves.emplace_back(from, to, MoveFlag::KNIGHT_PROMO);
        moves.emplace_back(from, to, MoveFlag::BISHOP_PROMO);
        moves.emplace_back(from, to, MoveFlag::ROOK_PROMO);
        moves.emplace_back(from, to, MoveFlag::QUEEN_PROMO);
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

void MoveGen::generatePawnMoves(const Position& position, MoveList& moves)
{
    Side side { position.getSideToMove() };
    Piece pawnPiece { side == Side::WHITE ? Piece::WHITE_PAWN : Piece::BLACK_PAWN };
    U64 enemyPieces { position.getPieceBitboard(side == Side::WHITE ? Piece::BLACK_ALL : Piece::WHITE_ALL) };
    U64 occupancy { position.getPieceBitboard(Piece::ALL_PIECES) };
    int pushDirection { side == Side::WHITE ? 8 : -8 };
    LERFSquare enPassantSquare { position.getEnPassantSquare() };

    U64 pawns { position.getPieceBitboard(pawnPiece) };
    while(pawns)
    {
        int fromSq { getLSBIndex(pawns) };
        LERFSquare from { static_cast<LERFSquare>(fromSq) };

        // Single push, and a double push from the starting rank if both squares ahead are empty.
        int oneSq { fromSq + pushDirection };
        if(!(occupancy & squareToBitboard(oneSq)))
        {
            addPawnMove(from, static_cast<LERFSquare>(oneSq), false, isPawnPromotionRank(side, oneSq), moves);

            int twoSq { oneSq + pushDirection };
            if(isPawnStartingRank(side, fromSq) && !(occupancy & squareToBitboard(twoSq)))
            {
                moves.emplace_back(from, static_cast<LERFSquare>(twoSq), MoveFlag::DOUBLE_PAWN_PUSH);
            }
        }

        // Diagonal captures, including promotion-captures on the far rank.
        U64 captureTargets { Attack::pawnAttacks(side, from) & enemyPieces };
        while(captureTargets)
        {
            int toSq { getLSBIndex(captureTargets) };
            addPawnMove(from, static_cast<LERFSquare>(toSq), true, isPawnPromotionRank(side, toSq), moves);
            captureTargets = resetBit(captureTargets, toSq);
        }

        // En passant: the target square is empty, so it can't be found via enemyPieces above.
        if(enPassantSquare != LERFSquare::NO_SQ && (Attack::pawnAttacks(side, from) & squareToBitboard(std::to_underlying(enPassantSquare))))
        {
            moves.emplace_back(from, enPassantSquare, MoveFlag::EP_CAPTURE);
        }

        pawns = resetBit(pawns, fromSq);
    }
}
