#ifndef MOVE_H
#define MOVE_H

#include "types.h" // LERFSquare, Piece, Side

#include <cstdint> // std::uint16_t
#include <string> // std::string
#include <vector> // std::vector

/*
 * The 4-bit move-kind flag packed into a Move. Bit meanings follow
 * the standard scheme: bit 3 marks a promotion, bit 2 marks a
 * capture, and bits 1-0 distinguish which of the four pieces a
 * promotion produces. See https://www.chessprogramming.org/Encoding_Moves.
 */
enum class MoveFlag : int
{
    QUIET, DOUBLE_PAWN_PUSH, KING_CASTLE, QUEEN_CASTLE, CAPTURE, EP_CAPTURE,
    KNIGHT_PROMO = 8, BISHOP_PROMO, ROOK_PROMO, QUEEN_PROMO,
    KNIGHT_PROMO_CAPTURE, BISHOP_PROMO_CAPTURE, ROOK_PROMO_CAPTURE, QUEEN_PROMO_CAPTURE
};

/*
 * A move packed into 16 bits: 6 bits "from" square, 6 bits "to"
 * square, 4 bits MoveFlag. See
 * https://www.chessprogramming.org/Encoding_Moves.
 */
class Move
{
    public:
        Move() = default;
        Move(LERFSquare from, LERFSquare to, MoveFlag flag);

        [[nodiscard]] bool operator==(const Move& other) const = default;

        [[nodiscard]] LERFSquare from() const;
        [[nodiscard]] LERFSquare to() const;
        [[nodiscard]] MoveFlag flag() const;

        [[nodiscard]] bool isCapture() const;
        [[nodiscard]] bool isPromotion() const;
        [[nodiscard]] bool isEnPassant() const;
        [[nodiscard]] bool isCastle() const;
        [[nodiscard]] bool isDoublePawnPush() const;

        // Precondition: isPromotion() is true.
        [[nodiscard]] Piece promotionPieceType(Side side) const;

        [[nodiscard]] std::string toUCIString() const;

    private:
        std::uint16_t encoded_ {};
};

using MoveList = std::vector<Move>;

#endif
