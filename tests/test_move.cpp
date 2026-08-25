#include "move.h" // Move, MoveFlag, MoveList
#include "doctest.h" // TEST_CASE, CHECK, CHECK_FALSE
#include "types.h" // LERFSquare, Piece, Side

TEST_CASE("Move round-trips from/to/flag for every MoveFlag at boundary squares")
{
    LERFSquare squaresToTest[] { LERFSquare::A1, LERFSquare::H1, LERFSquare::A8, LERFSquare::H8 };
    MoveFlag flagsToTest[] {
        MoveFlag::QUIET, MoveFlag::DOUBLE_PAWN_PUSH, MoveFlag::KING_CASTLE, MoveFlag::QUEEN_CASTLE,
        MoveFlag::CAPTURE, MoveFlag::EP_CAPTURE,
        MoveFlag::KNIGHT_PROMO, MoveFlag::BISHOP_PROMO, MoveFlag::ROOK_PROMO, MoveFlag::QUEEN_PROMO,
        MoveFlag::KNIGHT_PROMO_CAPTURE, MoveFlag::BISHOP_PROMO_CAPTURE, MoveFlag::ROOK_PROMO_CAPTURE, MoveFlag::QUEEN_PROMO_CAPTURE
    };

    for(LERFSquare from : squaresToTest)
    {
        for(LERFSquare to : squaresToTest)
        {
            for(MoveFlag flag : flagsToTest)
            {
                Move move { from, to, flag };
                CHECK(move.from() == from);
                CHECK(move.to() == to);
                CHECK(move.flag() == flag);
            }
        }
    }
}

TEST_CASE("Move equality compares from, to, and flag together")
{
    CHECK(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH) == Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH));
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH) == Move(LERFSquare::E2, LERFSquare::E3, MoveFlag::QUIET));
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH) == Move(LERFSquare::D2, LERFSquare::D4, MoveFlag::DOUBLE_PAWN_PUSH));
}

TEST_CASE("Move::isCapture is true exactly for CAPTURE, EP_CAPTURE, and promotion-capture flags")
{
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::QUIET).isCapture());
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH).isCapture());
    CHECK_FALSE(Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE).isCapture());
    CHECK_FALSE(Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE).isCapture());
    CHECK(Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE).isCapture());
    CHECK(Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE).isCapture());
    CHECK_FALSE(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::KNIGHT_PROMO).isCapture());
    CHECK_FALSE(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::BISHOP_PROMO).isCapture());
    CHECK_FALSE(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::ROOK_PROMO).isCapture());
    CHECK_FALSE(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO).isCapture());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::KNIGHT_PROMO_CAPTURE).isCapture());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::BISHOP_PROMO_CAPTURE).isCapture());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::ROOK_PROMO_CAPTURE).isCapture());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::QUEEN_PROMO_CAPTURE).isCapture());
}

TEST_CASE("Move::isPromotion is true exactly for the eight promotion flags")
{
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::QUIET).isPromotion());
    CHECK_FALSE(Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::KNIGHT_PROMO).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::BISHOP_PROMO).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::ROOK_PROMO).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::KNIGHT_PROMO_CAPTURE).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::BISHOP_PROMO_CAPTURE).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::ROOK_PROMO_CAPTURE).isPromotion());
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::QUEEN_PROMO_CAPTURE).isPromotion());
}

TEST_CASE("Move::isEnPassant is true only for EP_CAPTURE")
{
    CHECK(Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE).isEnPassant());
    CHECK_FALSE(Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE).isEnPassant());
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH).isEnPassant());
}

TEST_CASE("Move::isCastle is true only for KING_CASTLE and QUEEN_CASTLE")
{
    CHECK(Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE).isCastle());
    CHECK(Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE).isCastle());
    CHECK_FALSE(Move(LERFSquare::E1, LERFSquare::E2, MoveFlag::QUIET).isCastle());
    CHECK_FALSE(Move(LERFSquare::E1, LERFSquare::D1, MoveFlag::CAPTURE).isCastle());
}

TEST_CASE("Move::isDoublePawnPush is true only for DOUBLE_PAWN_PUSH")
{
    CHECK(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH).isDoublePawnPush());
    CHECK_FALSE(Move(LERFSquare::E2, LERFSquare::E3, MoveFlag::QUIET).isDoublePawnPush());
}

TEST_CASE("Move::promotionPieceType maps each promotion flag to the right piece for each side")
{
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::KNIGHT_PROMO).promotionPieceType(Side::WHITE) == Piece::WHITE_KNIGHT);
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::BISHOP_PROMO).promotionPieceType(Side::WHITE) == Piece::WHITE_BISHOP);
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::ROOK_PROMO).promotionPieceType(Side::WHITE) == Piece::WHITE_ROOK);
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO).promotionPieceType(Side::WHITE) == Piece::WHITE_QUEEN);
    CHECK(Move(LERFSquare::E2, LERFSquare::E1, MoveFlag::KNIGHT_PROMO).promotionPieceType(Side::BLACK) == Piece::BLACK_KNIGHT);
    CHECK(Move(LERFSquare::E2, LERFSquare::E1, MoveFlag::BISHOP_PROMO).promotionPieceType(Side::BLACK) == Piece::BLACK_BISHOP);
    CHECK(Move(LERFSquare::E2, LERFSquare::E1, MoveFlag::ROOK_PROMO).promotionPieceType(Side::BLACK) == Piece::BLACK_ROOK);
    CHECK(Move(LERFSquare::E2, LERFSquare::E1, MoveFlag::QUEEN_PROMO).promotionPieceType(Side::BLACK) == Piece::BLACK_QUEEN);
    // Promotion-captures map to the same piece as their non-capture counterparts.
    CHECK(Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::QUEEN_PROMO_CAPTURE).promotionPieceType(Side::WHITE) == Piece::WHITE_QUEEN);
}

TEST_CASE("Move::toUCIString formats quiet moves, captures, castles, and promotions")
{
    CHECK(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH).toUCIString() == "e2e4");
    CHECK(Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE).toUCIString() == "e4d5");
    CHECK(Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE).toUCIString() == "e1g1");
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO).toUCIString() == "e7e8q");
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::ROOK_PROMO).toUCIString() == "e7e8r");
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::BISHOP_PROMO).toUCIString() == "e7e8b");
    CHECK(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::KNIGHT_PROMO).toUCIString() == "e7e8n");
}

TEST_CASE("MoveList is an empty, appendable container by default")
{
    MoveList moves;
    CHECK(moves.empty());
    moves.push_back(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH));
    CHECK(moves.size() == 1);
}
