#ifndef TYPES_H
#define TYPES_H

//C++ Standard guarantees ULL to be AT LEAST 64 bits
using U64 = unsigned long long;

enum Piece : int
{
    EMPTY, 
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, 
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, 
    NUM_PIECES,
    WHITE_ALL = 13, BLACK_ALL, ALL_PIECES,
    NUM_PIECES_ALL
};

enum Side : int
{
    WHITE, BLACK, NUM_SIDES
};

enum Castle : int
{
    WHITE_KING_CASTLE = 1, WHITE_QUEEN_CASTLE = 2, 
    BLACK_KING_CASTLE = 4, BLACK_QUEEN_CASTLE = 8,
    NUM_CASTLE_STATES = 16
};

enum Rank : int
{
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NUM_RANKS
};

enum File : int
{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NUM_FILES
};

/*
 * Little-Endian Rank-File Mapping
 * 
 *     A  B  C  D  E  F  G  H
 * 
 * 8   56 57 58 59 60 61 62 63   8
 * 7   48 49 50 51 52 53 54 55   7
 * 6   40 41 42 43 44 45 46 47   6
 * 5   32 33 34 35 36 37 38 39   5
 * 4   24 25 26 27 28 29 30 31   4
 * 3   16 17 18 19 20 21 22 23   3
 * 2   8  9  10 11 12 13 14 15   2
 * 1   0  1  2  3  4  5  6  7    1
 * 
 *     A  B  C  D  E  F  G  H
 */
enum LERFSquare : int
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
    NO_SQ = 99
};

/*
 * Ray Directions for Little-Endian Rank-File Mapping
 * northwest    north   northeast
 * noWe         nort         noEa
 *         +7    +8    +9
 *             \  |  /
 * west    -1 <-  0 -> +1    east
 *             /  |  \
 *         -9    -8    -7
 * soWe         sout         soEa
 * southwest    south   southeast
 */
enum RayDirection : int
{
    NORTH = 8,
    NORTH_EAST = 9,
    EAST = 1,
    SOUTH_EAST = -7,
    SOUTH = -8,
    SOUTH_WEST = -9,
    WEST = -1,
    NORTH_WEST = 7
};

/*
 * Used for sliding piece attacks. See attack.cpp
 * and https://www.chessprogramming.org/Magic_Bitboards#Fancy
 */
struct FancyMagic
{
    U64* attackTablePointer {};
    U64 occupancyMask {};
    U64 magicNumber {};
    int shift {};
};

#endif