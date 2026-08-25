#include "move.h"
#include "types.h" // LERFSquare, Piece, Side

#include <cstdint> // std::uint16_t
#include <string> // std::string
#include <utility> // std::to_underlying

namespace
{

[[nodiscard]] char fileChar(LERFSquare square)
{
    int file { std::to_underlying(square) % 8 };
    return static_cast<char>('a' + file);
}

[[nodiscard]] char rankChar(LERFSquare square)
{
    int rank { std::to_underlying(square) / 8 };
    return static_cast<char>('1' + rank);
}

/*
 * Bits 1-0 of a promotion MoveFlag select which piece the pawn
 * promotes to, independent of whether the promotion is also a
 * capture (bit 2) -- see MoveFlag's doc comment in move.h.
 */
[[nodiscard]] char promotionChar(MoveFlag flag)
{
    int promotionPiece { std::to_underlying(flag) & 0b011 };
    switch(promotionPiece)
    {
        case 0: return 'n';
        case 1: return 'b';
        case 2: return 'r';
        default: return 'q';
    }
}

} // namespace

Move::Move(LERFSquare from, LERFSquare to, MoveFlag flag)
    : encoded_ { static_cast<std::uint16_t>(std::to_underlying(from) | (std::to_underlying(to) << 6) | (std::to_underlying(flag) << 12)) }
{
}

LERFSquare Move::from() const
{
    return static_cast<LERFSquare>(encoded_ & 0x3F);
}

LERFSquare Move::to() const
{
    return static_cast<LERFSquare>((encoded_ >> 6) & 0x3F);
}

MoveFlag Move::flag() const
{
    return static_cast<MoveFlag>((encoded_ >> 12) & 0xF);
}

/*
 * Bit 2 of the flag nibble marks a capture for every move kind that
 * involves one (CAPTURE, EP_CAPTURE, and all four promotion-capture
 * flags) -- see MoveFlag's doc comment in move.h.
 */
bool Move::isCapture() const
{
    return (std::to_underlying(flag()) & 0b0100) != 0;
}

/*
 * Bit 3 of the flag nibble marks every promotion flag, with or
 * without a capture -- see MoveFlag's doc comment in move.h.
 */
bool Move::isPromotion() const
{
    return (std::to_underlying(flag()) & 0b1000) != 0;
}

bool Move::isEnPassant() const
{
    return flag() == MoveFlag::EP_CAPTURE;
}

bool Move::isCastle() const
{
    return flag() == MoveFlag::KING_CASTLE || flag() == MoveFlag::QUEEN_CASTLE;
}

bool Move::isDoublePawnPush() const
{
    return flag() == MoveFlag::DOUBLE_PAWN_PUSH;
}

Piece Move::promotionPieceType(Side side) const
{
    int promotionPiece { std::to_underlying(flag()) & 0b011 };
    switch(promotionPiece)
    {
        case 0: return side == Side::WHITE ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT;
        case 1: return side == Side::WHITE ? Piece::WHITE_BISHOP : Piece::BLACK_BISHOP;
        case 2: return side == Side::WHITE ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        default: return side == Side::WHITE ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN;
    }
}

std::string Move::toUCIString() const
{
    std::string result;
    result += fileChar(from());
    result += rankChar(from());
    result += fileChar(to());
    result += rankChar(to());
    if(isPromotion())
    {
        result += promotionChar(flag());
    }
    return result;
}
