#include "doctest.h" // TEST_CASE, CHECK, REQUIRE
#include "position.h" // Position, FenParseError, describe, STANDARD_START_FEN
#include "types.h" // U64, Piece, Side, Castle, LERFSquare

#include <cstddef> // std::size_t
#include <iterator> // std::size

TEST_CASE("fromFen parses the standard start position")
{
    auto result { Position::fromFen(STANDARD_START_FEN) };
    REQUIRE(result.has_value());

    const Position& position { result.value() };
    CHECK(position.getPieceBitboard(Piece::WHITE_ROOK) == 0x81ULL); // A1 | H1
    CHECK(position.getSideToMove() == Side::WHITE);
    CHECK(position.getCastlingRights() == (Castle::WHITE_KING_CASTLE | Castle::WHITE_QUEEN_CASTLE | Castle::BLACK_KING_CASTLE | Castle::BLACK_QUEEN_CASTLE));
    CHECK(position.getEnPassantSquare() == LERFSquare::NO_SQ);
    CHECK(position.getFiftyMovesCount() == 0);
    CHECK(position.getPly() == 0);
}

TEST_CASE("fromFen decodes an en passant target square")
{
    // Position after 1. e4.
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1") };
    REQUIRE(result.has_value());

    const Position& position { result.value() };
    CHECK(position.getEnPassantSquare() == LERFSquare::E3);
    CHECK(position.getSideToMove() == Side::BLACK);
    CHECK(position.getPly() == 1);
}

TEST_CASE("fromFen rejects an invalid piece placement character")
{
    auto result { Position::fromFen("xnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidPiecePlacementChar);
}

TEST_CASE("fromFen rejects an out-of-range empty-square count")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/0/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidEmptySquareCount);
}

TEST_CASE("fromFen rejects an invalid active color character")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidActiveColorChar);
}

TEST_CASE("fromFen rejects a missing field separator")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR wKQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::MissingFieldSeparator);
}

TEST_CASE("fromFen rejects an invalid castling availability character")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w X - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidCastlingChar);
}

TEST_CASE("fromFen rejects an invalid en passant file")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq z3 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidEnPassantFile);
}

TEST_CASE("fromFen rejects an invalid en passant rank")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e9 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidEnPassantRank);
}

TEST_CASE("fromFen rejects an out-of-range halfmove clock")
{
    auto negative { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - -1 1") };
    REQUIRE(!negative.has_value());
    CHECK(negative.error() == FenParseError::InvalidHalfmoveClock);

    auto tooLarge { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 101 1") };
    REQUIRE(!tooLarge.has_value());
    CHECK(tooLarge.error() == FenParseError::InvalidHalfmoveClock);
}

TEST_CASE("fromFen rejects a fullmove number below 1")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidFullmoveNumber);
}

/*
 * NOTE: FenParseError::InvalidPieceChar and FenParseError::InvalidEnPassantSquare
 * are not exercised above. Reading fromFen() in src/position.cpp shows both
 * branches are currently unreachable through its public input: the piece
 * placement loop already rejects any character outside validPieceChars
 * before the InvalidPieceChar check ever runs, and the en passant file/rank
 * are already bounds-checked to 0..7 before the InvalidEnPassantSquare check,
 * making epSq > H8 impossible. This is a pre-existing dead-branch
 * observation, not something for this test suite to silently paper over.
 */
TEST_CASE("describe() has a real message for every FenParseError value")
{
    constexpr FenParseError allErrors[] {
        FenParseError::InvalidPiecePlacementChar,
        FenParseError::InvalidEmptySquareCount,
        FenParseError::InvalidPieceChar,
        FenParseError::InvalidActiveColorChar,
        FenParseError::MissingFieldSeparator,
        FenParseError::InvalidCastlingChar,
        FenParseError::InvalidEnPassantFile,
        FenParseError::InvalidEnPassantRank,
        FenParseError::InvalidEnPassantSquare,
        FenParseError::InvalidHalfmoveClock,
        FenParseError::InvalidFullmoveNumber
    };

    for(FenParseError error : allErrors)
    {
        CHECK(describe(error) != "unknown FEN parse error");
        CHECK(!describe(error).empty());
    }
}

TEST_CASE("calculatePositionHash is deterministic")
{
    auto first { Position::fromFen(STANDARD_START_FEN) };
    auto second { Position::fromFen(STANDARD_START_FEN) };
    REQUIRE(first.has_value());
    REQUIRE(second.has_value());

    CHECK(first->calculatePositionHash() == second->calculatePositionHash());
    CHECK(first->getPositionIdentity() == second->getPositionIdentity());
}

TEST_CASE("calculatePositionHash distinguishes positions differing in one dimension")
{
    const char* fens[] {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // start position
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", // side to move differs
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1", // castling rights differ
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1" // piece placement and en passant differ
    };

    U64 hashes[std::size(fens)] {};
    for(std::size_t i { 0 }; i < std::size(fens); ++i)
    {
        auto result { Position::fromFen(fens[i]) };
        REQUIRE(result.has_value());
        hashes[i] = result->calculatePositionHash();
    }

    for(std::size_t i { 0 }; i < std::size(fens); ++i)
    {
        for(std::size_t j { i + 1 }; j < std::size(fens); ++j)
        {
            CHECK(hashes[i] != hashes[j]);
        }
    }
}
