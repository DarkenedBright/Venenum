#include "doctest.h" // TEST_CASE, TEST_SUITE, CHECK, REQUIRE
#include "movegen.h" // MoveGen::perft
#include "position.h" // Position, STANDARD_START_FEN

#include <string> // std::string

namespace
{

[[nodiscard]] Position parsePosition(const std::string& fen)
{
    auto result { Position::fromFen(fen) };
    REQUIRE(result.has_value());
    return result.value();
}

} // namespace

/*
 * Deeper perft cases run into the millions of nodes, especially under
 * the debug build's ASan/UBSan instrumentation, so they're kept in
 * their own doctest suite: `make test` excludes it for a fast default
 * iteration loop, while `make test-perft` (or `make test-all`) runs
 * it. See the Makefile and CONTRIBUTING.md's Testing section.
 */
TEST_SUITE("perft")
{

/*
 * Node counts are the well-known perft reference values for the
 * starting position. See https://www.chessprogramming.org/Perft_Results.
 */
TEST_CASE("perft matches known node counts for the starting position")
{
    Position position { parsePosition(STANDARD_START_FEN) };

    CHECK(MoveGen::perft(position, 1) == 20);
    CHECK(MoveGen::perft(position, 2) == 400);
    CHECK(MoveGen::perft(position, 3) == 8902);
    CHECK(MoveGen::perft(position, 4) == 197281);
}

/*
 * Kiwipete is a standard perft stress position: it exercises castling
 * (both sides, both directions), en passant, and promotions together
 * from move 1. See https://www.chessprogramming.org/Perft_Results.
 */
TEST_CASE("perft matches known node counts for the Kiwipete position")
{
    Position position { parsePosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1") };

    CHECK(MoveGen::perft(position, 1) == 48);
    CHECK(MoveGen::perft(position, 2) == 2039);
    CHECK(MoveGen::perft(position, 3) == 97862);
}

TEST_CASE("perft matches known node counts for the position 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1")
{
    Position position { parsePosition("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1") };

    CHECK(MoveGen::perft(position, 1) == 14);
    CHECK(MoveGen::perft(position, 2) == 191);
    CHECK(MoveGen::perft(position, 3) == 2812);
    CHECK(MoveGen::perft(position, 4) == 43238);
}

TEST_CASE("perft matches known node counts for the position r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1")
{
    Position position { parsePosition("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1") };

    CHECK(MoveGen::perft(position, 1) == 6);
    CHECK(MoveGen::perft(position, 2) == 264);
    CHECK(MoveGen::perft(position, 3) == 9467);
    CHECK(MoveGen::perft(position, 4) == 422333);
}

TEST_CASE("perft matches known node counts for the position r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1")
{
    Position position { parsePosition("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1") };

    CHECK(MoveGen::perft(position, 1) == 6);
    CHECK(MoveGen::perft(position, 2) == 264);
    CHECK(MoveGen::perft(position, 3) == 9467);
    CHECK(MoveGen::perft(position, 4) == 422333);
}

TEST_CASE("perft matches known node counts for the position rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8")
{
    Position position { parsePosition("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8") };

    CHECK(MoveGen::perft(position, 1) == 44);
    CHECK(MoveGen::perft(position, 2) == 1486);
    CHECK(MoveGen::perft(position, 3) == 62379);
    CHECK(MoveGen::perft(position, 4) == 2103487);
}

TEST_CASE("perft matches known node counts for the position r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10")
{
    Position position { parsePosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10") };

    CHECK(MoveGen::perft(position, 1) == 46);
    CHECK(MoveGen::perft(position, 2) == 2079);
    CHECK(MoveGen::perft(position, 3) == 89890);
    CHECK(MoveGen::perft(position, 4) == 3894594);
}

} // TEST_SUITE("perft")
