#ifndef BOARD_H
#define BOARD_H

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

enum File
{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NUM_FILES
};

struct Board
{
    Piece piecesOnBoard[NUM_SQUARES] {};
    Square enPassantSquare { NO_SQ };
    int castlingRights { 15 };
    int fiftyMovesCount { 0 };
    int ply { 0 };
    uint64_t positionIdentity { 0 };
    Side sideToMove { WHITE };
};

inline const std::string STANDARD_START_FEN { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

Board fenToBoard(std::string fenString);
void outputBoardToConsole(Board board);

#endif