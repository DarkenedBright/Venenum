#ifndef POSITION_H
#define POSITION_H

#include <cstdint> //std::uint64_t
#include <string> //std::string

enum Square
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NUM_SQUARES,
    NO_SQ = 64
};

enum Piece
{
    EMPTY, 
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, 
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, 
    NUM_PIECES
};

inline const std::string pieceToChar { "-PNBRQKpnbrqk" };

enum Castle
{
    WHITE_KING_CASTLE = 1, WHITE_QUEEN_CASTLE = 2, 
    BLACK_KING_CASTLE = 4, BLACK_QUEEN_CASTLE = 8,
    NUM_CASTLE_STATES = 16
};

enum Side
{
    WHITE, BLACK, NUM_SIDES
};

enum Rank
{
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NUM_RANKS
};

inline const std::string rankToChar { "12345678" };

enum File
{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NUM_FILES
};

inline const std::string fileToChar { "abcdefgh" };

inline const std::string STANDARD_START_FEN { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

class Position
{
    private:
        //Static Zobrist keys, initialized with initPositionZobristKeys()
        inline static constexpr std::uint64_t POSITION_ZOBRIST_SEED { 0xFD2D8157399E58D4 };
        inline static std::uint64_t pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
        inline static std::uint64_t sideToMoveKey;
        inline static std::uint64_t castlingRightKeys[NUM_CASTLE_STATES];
        inline static std::uint64_t enPassantFileKeys[NUM_FILES];

        //Position member variables
        Piece piecesOnBoard[NUM_SQUARES] {};
        Square enPassantSquare {};
        int castlingRights {};
        int fiftyMovesCount {};
        int ply {};
        std::uint64_t positionIdentity {};
        Side sideToMove {};
    public:
        static void initZobristPositionKeys();
        Position(const std::string& fenString);
        std::uint64_t calculatePositionHash();
        void print();
};

#endif