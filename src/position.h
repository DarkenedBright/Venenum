#ifndef POSITION_H
#define POSITION_H

#include "types.h" //LERFSquare, Piece, File, Rank, Castle, Side, U64

#include <string> //std::string

inline const std::string pieceToChar { "-PNBRQKpnbrqk" };
inline const std::string rankToChar { "12345678" };
inline const std::string fileToChar { "abcdefgh" };

inline const std::string STANDARD_START_FEN { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

class Position
{
    private:
        //Static Zobrist keys, initialized with initPositionZobristKeys()
        inline static constexpr U64 POSITION_ZOBRIST_SEED { 0xFD2D8157399E58D4 };
        inline static U64 pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
        inline static U64 sideToMoveKey;
        inline static U64 castlingRightKeys[NUM_CASTLE_STATES];
        inline static U64 enPassantFileKeys[NUM_FILES];

        //Position member variables
        Piece piecesOnBoard[NUM_SQUARES] {};
        LERFSquare enPassantSquare {};
        int castlingRights {};
        int fiftyMovesCount {};
        int ply {};
        U64 positionIdentity {};
        Side sideToMove {};
    public:
        static void initZobristPositionKeys();
        Position(const std::string& fenString);
        U64 calculatePositionHash();
        void print();
};

#endif