#include "bitboard.h" // squareToBitboard
#include "doctest.h" // TEST_CASE, CHECK, REQUIRE
#include "move.h" // Move, MoveFlag
#include "position.h" // Position, FenParseError, describe, STANDARD_START_FEN, UnmakeState
#include "test_helpers.h" // parsePosition
#include "types.h" // U64, Piece, Side, Castle, LERFSquare

#include <cstddef> // std::size_t
#include <iterator> // std::size
#include <string_view> // std::string_view
#include <utility> // std::to_underlying

namespace
{

// White pawn on e5 can capture en passant onto d6 against a black
// pawn that just double-pushed to d5.
constexpr std::string_view EN_PASSANT_READY_FEN { "8/8/8/3pP3/8/8/8/8 w - d6 0 1" };

} // namespace

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

TEST_CASE("fromFen rejects a rank with too many squares")
{
    // Rank 8 describes 9 squares (8 pawns plus a trailing empty square).
    auto result { Position::fromFen("pppppppp1/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidPiecePlacementRankLength);
}

TEST_CASE("fromFen rejects a rank with too few squares")
{
    // Rank 8 describes only 7 squares before the separator.
    auto result { Position::fromFen("ppppppp/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidPiecePlacementRankLength);
}

TEST_CASE("fromFen rejects too few ranks")
{
    auto result { Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/8 w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidPiecePlacementRankCount);
}

TEST_CASE("fromFen rejects too many ranks")
{
    // A 9th rank used to walk `sq` negative and hit undefined behavior in squareToBitboard().
    auto result { Position::fromFen("8/8/8/8/8/8/8/8/P w KQkq - 0 1") };
    REQUIRE(!result.has_value());
    CHECK(result.error() == FenParseError::InvalidPiecePlacementRankCount);
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
        FenParseError::InvalidPiecePlacementRankLength,
        FenParseError::InvalidPiecePlacementRankCount,
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

TEST_CASE("makeMove on a quiet move relocates the piece, flips side to move, and advances ply/fifty-move count")
{
    Position position { parsePosition(STANDARD_START_FEN) };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::B1, LERFSquare::C3, MoveFlag::QUIET)) };

    CHECK((position.getPieceBitboard(Piece::WHITE_KNIGHT) & squareToBitboard(std::to_underlying(LERFSquare::C3))) != 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_KNIGHT) & squareToBitboard(std::to_underlying(LERFSquare::B1))) == 0);
    CHECK(position.getSideToMove() == Side::BLACK);
    CHECK(position.getPly() == 1);
    CHECK(position.getEnPassantSquare() == LERFSquare::NO_SQ);
    CHECK(position.getFiftyMovesCount() == 1);
}

TEST_CASE("makeMove on a double pawn push sets the en passant square and resets the fifty-move count")
{
    Position position { parsePosition(STANDARD_START_FEN) };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH)) };

    CHECK((position.getPieceBitboard(Piece::WHITE_PAWN) & squareToBitboard(std::to_underlying(LERFSquare::E4))) != 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_PAWN) & squareToBitboard(std::to_underlying(LERFSquare::E2))) == 0);
    CHECK(position.getEnPassantSquare() == LERFSquare::E3);
    CHECK(position.getFiftyMovesCount() == 0);
}

TEST_CASE("makeMove on a capture removes the captured piece and reports it in the UnmakeState")
{
    Position position { parsePosition("8/8/8/3n4/4P3/8/8/8 w - - 12 1") };
    UnmakeState saved { position.makeMove(Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE)) };

    CHECK(saved.capturedPiece == Piece::BLACK_KNIGHT);
    CHECK((position.getPieceBitboard(Piece::WHITE_PAWN) & squareToBitboard(std::to_underlying(LERFSquare::D5))) != 0);
    CHECK(position.getPieceBitboard(Piece::BLACK_KNIGHT) == 0);
    CHECK(position.getFiftyMovesCount() == 0);
}

TEST_CASE("makeMove on an en passant capture removes the pawn behind the destination square")
{
    Position position { parsePosition(EN_PASSANT_READY_FEN) };
    UnmakeState saved { position.makeMove(Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE)) };

    CHECK(saved.capturedPiece == Piece::BLACK_PAWN);
    CHECK((position.getPieceBitboard(Piece::WHITE_PAWN) & squareToBitboard(std::to_underlying(LERFSquare::D6))) != 0);
    CHECK(position.getPieceBitboard(Piece::BLACK_PAWN) == 0);
    CHECK(position.getEnPassantSquare() == LERFSquare::NO_SQ);
}

TEST_CASE("makeMove on a kingside castle moves the rook alongside the king and clears both castling rights")
{
    Position position { parsePosition("8/8/8/8/8/8/8/4K2R w KQ - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE)) };

    CHECK((position.getPieceBitboard(Piece::WHITE_KING) & squareToBitboard(std::to_underlying(LERFSquare::G1))) != 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_ROOK) & squareToBitboard(std::to_underlying(LERFSquare::F1))) != 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_ROOK) & squareToBitboard(std::to_underlying(LERFSquare::H1))) == 0);
    CHECK(std::to_underlying(position.getCastlingRights()) == 0);
}

TEST_CASE("makeMove on a queenside castle moves the rook alongside the king")
{
    Position position { parsePosition("r3k3/8/8/8/8/8/8/8 b q - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::E8, LERFSquare::C8, MoveFlag::QUEEN_CASTLE)) };

    CHECK((position.getPieceBitboard(Piece::BLACK_KING) & squareToBitboard(std::to_underlying(LERFSquare::C8))) != 0);
    CHECK((position.getPieceBitboard(Piece::BLACK_ROOK) & squareToBitboard(std::to_underlying(LERFSquare::D8))) != 0);
    CHECK((position.getPieceBitboard(Piece::BLACK_ROOK) & squareToBitboard(std::to_underlying(LERFSquare::A8))) == 0);
    CHECK(std::to_underlying(position.getCastlingRights()) == 0);
}

TEST_CASE("makeMove on a promotion replaces the pawn with the promoted piece")
{
    Position position { parsePosition("8/4P3/8/8/8/8/8/8 w - - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO)) };

    CHECK(position.getPieceBitboard(Piece::WHITE_PAWN) == 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_QUEEN) & squareToBitboard(std::to_underlying(LERFSquare::E8))) != 0);
    CHECK(position.getFiftyMovesCount() == 0);
}

TEST_CASE("makeMove on a promotion capture replaces the pawn and removes the captured piece")
{
    Position position { parsePosition("4n3/3P4/8/8/8/8/8/8 w - - 0 1") };
    UnmakeState saved { position.makeMove(Move(LERFSquare::D7, LERFSquare::E8, MoveFlag::QUEEN_PROMO_CAPTURE)) };

    CHECK(saved.capturedPiece == Piece::BLACK_KNIGHT);
    CHECK(position.getPieceBitboard(Piece::WHITE_PAWN) == 0);
    CHECK((position.getPieceBitboard(Piece::WHITE_QUEEN) & squareToBitboard(std::to_underlying(LERFSquare::E8))) != 0);
    CHECK(position.getPieceBitboard(Piece::BLACK_KNIGHT) == 0);
}

TEST_CASE("makeMove clears a rook's castling right when it is captured on its home square")
{
    Position position { parsePosition("r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::A1, LERFSquare::A8, MoveFlag::CAPTURE)) };

    CHECK(position.getCastlingRights() == Castle::WHITE_KING_CASTLE);
}

TEST_CASE("makeMove clears both of a side's castling rights on a plain king move, not just an actual castle")
{
    Position position { parsePosition("r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::E1, LERFSquare::E2, MoveFlag::QUIET)) };

    CHECK(position.getCastlingRights() == Castle::BLACK_QUEEN_CASTLE);
}

TEST_CASE("makeMove clears only the moved rook's side when it moves off its home square without being captured")
{
    Position position { parsePosition("r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::A1, LERFSquare::B1, MoveFlag::QUIET)) };

    CHECK(position.getCastlingRights() == (Castle::WHITE_KING_CASTLE | Castle::BLACK_QUEEN_CASTLE));
}

TEST_CASE("makeMove increments the fifty-move count on a non-pawn, non-capture move")
{
    Position position { parsePosition("8/8/8/8/8/8/8/N7 w - - 5 1") };
    [[maybe_unused]] UnmakeState saved { position.makeMove(Move(LERFSquare::A1, LERFSquare::B3, MoveFlag::QUIET)) };

    CHECK(position.getFiftyMovesCount() == 6);
}

TEST_CASE("unmakeMove is a perfect inverse of makeMove across every move kind")
{
    struct Case { std::string_view fen; Move move; };
    Case cases[] {
        { STANDARD_START_FEN, Move(LERFSquare::B1, LERFSquare::C3, MoveFlag::QUIET) },
        { STANDARD_START_FEN, Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH) },
        { "8/8/8/3n4/4P3/8/8/8 w - - 12 1", Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE) },
        { EN_PASSANT_READY_FEN, Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE) },
        { "8/8/8/8/8/8/8/4K2R w KQ - 0 1", Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE) },
        { "r3k3/8/8/8/8/8/8/8 b q - 0 1", Move(LERFSquare::E8, LERFSquare::C8, MoveFlag::QUEEN_CASTLE) },
        { "8/4P3/8/8/8/8/8/8 w - - 0 1", Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO) },
        { "4n3/3P4/8/8/8/8/8/8 w - - 0 1", Move(LERFSquare::D7, LERFSquare::E8, MoveFlag::QUEEN_PROMO_CAPTURE) },
        { "r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1", Move(LERFSquare::A1, LERFSquare::A8, MoveFlag::CAPTURE) },
    };

    for(const Case& testCase : cases)
    {
        Position before { parsePosition(testCase.fen) };
        Position after { before };
        UnmakeState saved { after.makeMove(testCase.move) };
        after.unmakeMove(testCase.move, saved);

        for(int pieceType { 0 }; pieceType < std::to_underlying(Piece::NUM_PIECES_ALL); ++pieceType)
        {
            Piece piece { static_cast<Piece>(pieceType) };
            CHECK(after.getPieceBitboard(piece) == before.getPieceBitboard(piece));
        }
        CHECK(after.getSideToMove() == before.getSideToMove());
        CHECK(after.getCastlingRights() == before.getCastlingRights());
        CHECK(after.getEnPassantSquare() == before.getEnPassantSquare());
        CHECK(after.getFiftyMovesCount() == before.getFiftyMovesCount());
        CHECK(after.getPly() == before.getPly());
        CHECK(after.getPositionIdentity() == before.getPositionIdentity());
    }
}

TEST_CASE("makeMove keeps getPositionIdentity in sync with a fresh calculatePositionHash")
{
    struct Case { std::string_view fen; Move move; };
    Case cases[] {
        { STANDARD_START_FEN, Move(LERFSquare::B1, LERFSquare::C3, MoveFlag::QUIET) },
        { STANDARD_START_FEN, Move(LERFSquare::E2, LERFSquare::E4, MoveFlag::DOUBLE_PAWN_PUSH) },
        { "8/8/8/3n4/4P3/8/8/8 w - - 12 1", Move(LERFSquare::E4, LERFSquare::D5, MoveFlag::CAPTURE) },
        { EN_PASSANT_READY_FEN, Move(LERFSquare::E5, LERFSquare::D6, MoveFlag::EP_CAPTURE) },
        { "8/8/8/8/8/8/8/4K2R w KQ - 0 1", Move(LERFSquare::E1, LERFSquare::G1, MoveFlag::KING_CASTLE) },
        { "r3k3/8/8/8/8/8/8/8 b q - 0 1", Move(LERFSquare::E8, LERFSquare::C8, MoveFlag::QUEEN_CASTLE) },
        { "8/4P3/8/8/8/8/8/8 w - - 0 1", Move(LERFSquare::E7, LERFSquare::E8, MoveFlag::QUEEN_PROMO) },
        { "4n3/3P4/8/8/8/8/8/8 w - - 0 1", Move(LERFSquare::D7, LERFSquare::E8, MoveFlag::QUEEN_PROMO_CAPTURE) },
        // Rook captured on its home square: exercises the castling-rights key transition.
        { "r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1", Move(LERFSquare::A1, LERFSquare::A8, MoveFlag::CAPTURE) },
        // Plain king move: clears both castling rights on its side without castling.
        { "r3k3/8/8/8/8/8/8/R3K3 w KQq - 0 1", Move(LERFSquare::E1, LERFSquare::E2, MoveFlag::QUIET) },
    };

    for(const Case& testCase : cases)
    {
        Position position { parsePosition(testCase.fen) };
        [[maybe_unused]] UnmakeState saved { position.makeMove(testCase.move) };
        CHECK(position.getPositionIdentity() == position.calculatePositionHash());
    }
}
