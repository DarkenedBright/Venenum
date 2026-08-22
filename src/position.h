#ifndef POSITION_H
#define POSITION_H

#include "types.h" //LERFSquare, Piece, File, Rank, Castle, Side, U64

#include <expected> // std::expected, std::unexpected
#include <string> //std::string
#include <string_view> // std::string_view
#include <utility> // std::to_underlying

inline const std::string pieceToChar { "-PNBRQKpnbrqk" };
inline const std::string rankToChar { "12345678" };
inline const std::string fileToChar { "abcdefgh" };

inline const std::string STANDARD_START_FEN { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

enum class FenParseError
{
    InvalidPiecePlacementChar,
    InvalidEmptySquareCount,
    InvalidPieceChar,
    InvalidPiecePlacementRankLength,
    InvalidPiecePlacementRankCount,
    InvalidActiveColorChar,
    MissingFieldSeparator,
    InvalidCastlingChar,
    InvalidEnPassantFile,
    InvalidEnPassantRank,
    InvalidEnPassantSquare,
    InvalidHalfmoveClock,
    InvalidFullmoveNumber
};

[[nodiscard]] constexpr std::string_view describe(FenParseError error)
{
    switch(error)
    {
        case FenParseError::InvalidPiecePlacementChar: return "invalid FEN piece placement character";
        case FenParseError::InvalidEmptySquareCount: return "invalid FEN piece placement: empty-square count out of range";
        case FenParseError::InvalidPieceChar: return "invalid FEN piece character";
        case FenParseError::InvalidPiecePlacementRankLength: return "invalid FEN piece placement: rank does not contain exactly 8 squares";
        case FenParseError::InvalidPiecePlacementRankCount: return "invalid FEN piece placement: expected exactly 8 ranks";
        case FenParseError::InvalidActiveColorChar: return "invalid FEN active color character, expected 'w' or 'b'";
        case FenParseError::MissingFieldSeparator: return "invalid FEN: expected space between fields";
        case FenParseError::InvalidCastlingChar: return "invalid FEN castling availability character";
        case FenParseError::InvalidEnPassantFile: return "invalid FEN en passant file character";
        case FenParseError::InvalidEnPassantRank: return "invalid FEN en passant rank character";
        case FenParseError::InvalidEnPassantSquare: return "invalid FEN en passant square";
        case FenParseError::InvalidHalfmoveClock: return "invalid FEN halfmove clock, must be 0-100";
        case FenParseError::InvalidFullmoveNumber: return "invalid FEN fullmove number, must be >= 1";
    }
    return "unknown FEN parse error";
}

class Position
{
    private:
        // Static Zobrist keys, initialized with initPositionZobristKeys()
        inline static constexpr U64 POSITION_ZOBRIST_SEED { 0xFD2D8157399E58D4 };
        inline static U64 pieceSquareKeys[std::to_underlying(LERFSquare::NUM_SQUARES)][std::to_underlying(Piece::NUM_PIECES)];
        inline static U64 sideToMoveKey;
        inline static U64 castlingRightKeys[std::to_underlying(Castle::NUM_CASTLE_STATES)];
        inline static U64 enPassantFileKeys[std::to_underlying(File::NUM_FILES)];

        // Position member variables
        U64 pieceBitboards[std::to_underlying(Piece::NUM_PIECES_ALL)] {};
        LERFSquare enPassantSquare {};
        Castle castlingRights {};
        int fiftyMovesCount {};
        int ply {};
        U64 positionIdentity {};
        Side sideToMove {};
    public:
        static void initZobristPositionKeys();
        Position() = default;
        [[nodiscard]] static std::expected<Position, FenParseError> fromFen(const std::string& fenString);
        [[nodiscard]] U64 calculatePositionHash() const;
        void print() const;

        [[nodiscard]] U64 getPieceBitboard(Piece piece) const;
        [[nodiscard]] Side getSideToMove() const;
        [[nodiscard]] Castle getCastlingRights() const;
        [[nodiscard]] LERFSquare getEnPassantSquare() const;
        [[nodiscard]] int getFiftyMovesCount() const;
        [[nodiscard]] int getPly() const;
        [[nodiscard]] U64 getPositionIdentity() const;
};

#endif