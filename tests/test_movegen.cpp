#include "doctest.h" // TEST_CASE, CHECK, REQUIRE
#include "move.h" // Move, MoveFlag, MoveList
#include "movegen.h" // MoveGen::generateKnightMoves, MoveGen::generateKingMoves, MoveGen::generateSlidingMoves, MoveGen::generatePawnMoves
#include "position.h" // Position
#include "types.h" // LERFSquare

namespace
{

[[nodiscard]] Position parsePosition(const std::string& fen)
{
    auto result { Position::fromFen(fen) };
    REQUIRE(result.has_value());
    return result.value();
}

[[nodiscard]] int countQuiet(const MoveList& moves)
{
    int count { 0 };
    for(const Move& move : moves)
    {
        if(move.flag() == MoveFlag::QUIET) ++count;
    }
    return count;
}

[[nodiscard]] int countCaptures(const MoveList& moves)
{
    int count { 0 };
    for(const Move& move : moves)
    {
        if(move.isCapture()) ++count;
    }
    return count;
}

[[nodiscard]] bool contains(const MoveList& moves, Move expected)
{
    for(const Move& move : moves)
    {
        if(move == expected) return true;
    }
    return false;
}

[[nodiscard]] int countPromotions(const MoveList& moves)
{
    int count { 0 };
    for(const Move& move : moves)
    {
        if(move.isPromotion()) ++count;
    }
    return count;
}

} // namespace

TEST_CASE("generateKnightMoves produces 8 quiet moves for a lone knight in the center")
{
    Position position { parsePosition("8/8/8/8/3N4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateKnightMoves(position, moves);

    CHECK(moves.size() == 8);
    CHECK(countQuiet(moves) == 8);
}

TEST_CASE("generateKnightMoves produces 2 quiet moves for a lone knight in the corner")
{
    Position position { parsePosition("8/8/8/8/8/8/8/N7 w - - 0 1") };
    MoveList moves;
    MoveGen::generateKnightMoves(position, moves);

    CHECK(moves.size() == 2);
    CHECK(countQuiet(moves) == 2);
}

TEST_CASE("generateKnightMoves flags an enemy-occupied destination as a capture and excludes an own-occupied one")
{
    Position position { parsePosition("8/8/8/1p3P2/3N4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateKnightMoves(position, moves);

    CHECK(moves.size() == 7);
    CHECK(countCaptures(moves) == 1);
    CHECK(contains(moves, Move(LERFSquare::D4, LERFSquare::B5, MoveFlag::CAPTURE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::F5, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::F5, MoveFlag::CAPTURE)));
}

TEST_CASE("generateKingMoves produces 8 quiet moves for a lone king in the center")
{
    Position position { parsePosition("8/8/8/8/4K3/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK(moves.size() == 8);
    CHECK(countQuiet(moves) == 8);
}

TEST_CASE("generateKingMoves flags an enemy-occupied neighbor as a capture and excludes an own-occupied one")
{
    Position position { parsePosition("8/8/8/4P3/4K3/3p4/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK(moves.size() == 7);
    CHECK(countCaptures(moves) == 1);
    CHECK(contains(moves, Move(LERFSquare::E4, LERFSquare::D3, MoveFlag::CAPTURE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::E4, LERFSquare::E5, MoveFlag::QUIET)));
}

TEST_CASE("generateKingMoves generates both castle moves when rights, squares, and safety all allow it")
{
    Position position { parsePosition("8/8/8/8/8/8/8/R3K2R w KQkq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK(contains(moves, Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)));
    CHECK(contains(moves, Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE)));
}

TEST_CASE("generateKingMoves excludes queenside castling when the B-file square is occupied, but keeps kingside")
{
    Position position { parsePosition("8/8/8/8/8/8/8/RN2K2R w KQkq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK_FALSE(contains(moves, Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE)));
    CHECK(contains(moves, Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)));
}

TEST_CASE("generateKingMoves excludes kingside castling when the transit square is attacked, even though it's empty")
{
    Position position { parsePosition("5r2/8/8/8/8/8/8/R3K2R w KQkq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK_FALSE(contains(moves, Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)));
    CHECK(contains(moves, Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE)));
}

TEST_CASE("generateKingMoves excludes both castle moves when the king is in check")
{
    Position position { parsePosition("4r3/8/8/8/8/8/8/R3K2R w KQkq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK_FALSE(contains(moves, Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE)));
}

TEST_CASE("generateKingMoves only generates the castle side actually granted by castlingRights")
{
    Position position { parsePosition("8/8/8/8/8/8/8/R3K2R w Kq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK(contains(moves, Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::E1, LERFSquare::C1, MoveFlag::QUEEN_CASTLE)));
}

TEST_CASE("generateKingMoves generates both castle moves for black symmetrically")
{
    Position position { parsePosition("r3k2r/8/8/8/8/8/8/8 b KQkq - 0 1") };
    MoveList moves;
    MoveGen::generateKingMoves(position, moves);

    CHECK(contains(moves, Move(LERFSquare::E8, LERFSquare::G8, MoveFlag::KING_CASTLE)));
    CHECK(contains(moves, Move(LERFSquare::E8, LERFSquare::C8, MoveFlag::QUEEN_CASTLE)));
}

TEST_CASE("generateSlidingMoves produces 14 quiet moves for a lone rook in the open")
{
    Position position { parsePosition("8/8/8/8/3R4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(moves.size() == 14);
    CHECK(countQuiet(moves) == 14);
}

TEST_CASE("generateSlidingMoves stops a rook at the first blocker in each direction")
{
    Position position { parsePosition("8/8/3P4/8/3R1p2/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(moves.size() == 9);
    CHECK(countQuiet(moves) == 8);
    CHECK(countCaptures(moves) == 1);
    CHECK(contains(moves, Move(LERFSquare::D4, LERFSquare::D5, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::D4, LERFSquare::F4, MoveFlag::CAPTURE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::D6, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::D7, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::G4, MoveFlag::QUIET)));
}

TEST_CASE("generateSlidingMoves produces 13 quiet moves for a lone bishop in the center")
{
    Position position { parsePosition("8/8/8/8/3B4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(moves.size() == 13);
    CHECK(countQuiet(moves) == 13);
}

TEST_CASE("generateSlidingMoves stops a bishop at the first blocker in each diagonal direction")
{
    Position position { parsePosition("8/8/5p2/8/3B4/8/1P6/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(moves.size() == 9);
    CHECK(countQuiet(moves) == 8);
    CHECK(countCaptures(moves) == 1);
    CHECK(contains(moves, Move(LERFSquare::D4, LERFSquare::F6, MoveFlag::CAPTURE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::G7, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::B2, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::A1, MoveFlag::QUIET)));
}

TEST_CASE("generateSlidingMoves produces 27 quiet moves for a lone queen in the center")
{
    Position position { parsePosition("8/8/8/8/3Q4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(moves.size() == 27);
    CHECK(countQuiet(moves) == 27);
}

TEST_CASE("generateSlidingMoves respects blockers on both the orthogonal and diagonal lines of a queen")
{
    Position position { parsePosition("8/8/3P1p2/8/3Q4/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generateSlidingMoves(position, moves);

    CHECK(countCaptures(moves) == 1);
    CHECK(contains(moves, Move(LERFSquare::D4, LERFSquare::F6, MoveFlag::CAPTURE)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::D6, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::D7, MoveFlag::QUIET)));
    CHECK_FALSE(contains(moves, Move(LERFSquare::D4, LERFSquare::G7, MoveFlag::QUIET)));
}

TEST_CASE("generatePawnMoves produces a single push and a double push from the starting rank")
{
    Position position { parsePosition("8/8/8/8/8/8/4P3/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 2);
    CHECK(contains(moves, Move(LERFSquare::E2, LERFSquare::E3, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH)));
}

TEST_CASE("generatePawnMoves generates no push (single or double) when the square ahead is blocked")
{
    Position position { parsePosition("8/8/8/8/8/4p3/4P3/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.empty());
}

TEST_CASE("generatePawnMoves produces all four promotion flags for a push onto the last rank")
{
    Position position { parsePosition("8/4P3/8/8/8/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 4);
    CHECK(countPromotions(moves) == 4);
    CHECK(countCaptures(moves) == 0);
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::KNIGHT_PROMO)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::BISHOP_PROMO)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::ROOK_PROMO)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO)));
}

TEST_CASE("generatePawnMoves flags non-promotion-rank diagonal captures as CAPTURE alongside an unblocked push")
{
    Position position { parsePosition("8/8/3p1p2/4P3/8/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 3);
    CHECK(countQuiet(moves) == 1);
    CHECK(countCaptures(moves) == 2);
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::E6, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::CAPTURE)));
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::F6, MoveFlag::CAPTURE)));
}

TEST_CASE("generatePawnMoves produces all four promotion-capture flags for a capture onto the last rank")
{
    Position position { parsePosition("3n1n2/4P3/8/8/8/8/8/8 w - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 12);
    CHECK(countPromotions(moves) == 12);
    CHECK(countCaptures(moves) == 8);
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::D8, MoveFlag::QUEEN_PROMO_CAPTURE)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::F8, MoveFlag::KNIGHT_PROMO_CAPTURE)));
}

TEST_CASE("generatePawnMoves produces an en passant capture when the target square matches getEnPassantSquare")
{
    Position position { parsePosition("8/8/8/3pP3/8/8/8/8 w - d6 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 2);
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::E6, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE)));
}

TEST_CASE("generatePawnMoves pushes black pawns toward rank 1")
{
    Position position { parsePosition("8/4p3/8/8/8/8/8/8 b - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 2);
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E6, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::E7, LERFSquare::E5, MoveFlag::DOUBLE_PAWN_PUSH)));
}

TEST_CASE("generatePawnMoves captures diagonally toward rank 1 for black")
{
    Position position { parsePosition("8/8/8/4p3/3P1P2/8/8/8 b - - 0 1") };
    MoveList moves;
    MoveGen::generatePawnMoves(position, moves);

    CHECK(moves.size() == 3);
    CHECK(countQuiet(moves) == 1);
    CHECK(countCaptures(moves) == 2);
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::E4, MoveFlag::QUIET)));
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::D4, MoveFlag::CAPTURE)));
    CHECK(contains(moves, Move(LERFSquare::E5, LERFSquare::F4, MoveFlag::CAPTURE)));
}
