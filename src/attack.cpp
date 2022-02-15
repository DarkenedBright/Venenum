#include "attack.h"
#include "bitboard.h" // squareToBitboard()
#include "types.h" // U64, File, Rank, LERFSquare, RayDirection, FancyMagic

/*
 * Return valid if a slide move of a bishop or rook
 * stayed on the board, and did not wrap around the board
 * to an opposite side file or rank.
 */
bool slideIsValid(int from, int to)
{
    File fromFile { from % NUM_FILES };
    File toFile { to % NUM_FILES };
    int fileDistance { fromFile - toFile };
    
    Rank fromRank { from / NUM_RANKS };
    Rank toRank { to / NUM_RANKS };
    int rankDistance { fromRank - toRank };

    return to >= A1 && to < NUM_SQUARES && fileDistance > -2 && fileDistance < 2 && rankDistance > -2 && rankDistance < 2;
}

/*
 * Return a bitboard containing all attacked
 * squares on the board from a rook on sq with
 * a relevant occupancy. This includes attacked
 * squares with blocker pieces on them.
 */
U64 calculateRookAttacks(int sq, U64 occupancy)
{
    U64 attack { 0ULL };
    RayDirection rookDirections[4] { NORTH, EAST, SOUTH, WEST };
    for(RayDirection dir: rookDirections)
    {
        int curSq { sq + dir };
        int prevSq { sq };
        U64 bbSq { squareToBitboard(sq) };
        while(slideIsValid(prevSq, curSq))
        {
            attack |= bbSq;

            if(occupancy & bbSq) break;

            prevSq = curSq;
            curSq += dir;
            bbSq = squareToBitboard(curSq);
        }
    }

    return attack;
}

/*
 * Loop through all the squares, and initialize attack bitboards
 * for rook pieces with all possible relevant occupancies for that
 * square. The traversal of all subsets of a specific occupancy uses
 * the formula a = (a - b) & b. Called the Carry-Rippler method, introduced
 * by Marcel van Kervinck and later by Steffan Westcott.
 */
void initRookAttacks()
{
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        FancyMagic& curMagic = ROOK_FANCY_MAGICS[sq];
        curMagic.shift = ROOK_SHIFT[sq];
        curMagic.magicNumber = ROOK_MAGIC_NUMBERS[sq];
        curMagic.occupancyMask = ROOK_OCCUPANCY[sq];

        // Calculate pointer address into ROOK_ATTACKS_TABLE for sq
        if(sq == A1)
        {
            curMagic.attackTablePointer = ROOK_ATTACKS_TABLE;
        }
        else
        {
            U64* previousSqPointer = ROOK_FANCY_MAGICS[sq - 1].attackTablePointer;
            int fancyBitsUsed = 64 - ROOK_SHIFT[sq - 1];
            int pointerOffset = 1ULL << fancyBitsUsed;
            curMagic.attackTablePointer = previousSqPointer + pointerOffset;
        }
        

        U64 currentOccupancy { 0ULL };
        do
        {
            U64 curIndex { (currentOccupancy * curMagic.magicNumber) >> curMagic.shift };
            U64 curAttack { calculateRookAttacks(sq, currentOccupancy) };

            curMagic.attackTablePointer[curIndex] = curAttack;

            currentOccupancy = (currentOccupancy - curMagic.occupancyMask) & curMagic.occupancyMask;
        } while (currentOccupancy);
    }
}

/*
 * Return a bitboard containing all attacked
 * squares on the board from a bishop on sq with
 * a relevant occupancy. This includes attacked
 * squares with blocker pieces on them.
 */
U64 calculateBishopAttacks(int sq, U64 occupancy)
{
    U64 attack { 0ULL };
    RayDirection bishopDirections[4] { NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST };
    for(RayDirection dir: bishopDirections)
    {
        int curSq { sq + dir };
        int prevSq { sq };
        U64 bbSq { squareToBitboard(curSq) };
        while(slideIsValid(prevSq, curSq))
        {
            attack |= bbSq;

            if(occupancy & bbSq) break;

            prevSq = curSq;
            curSq += dir;
            bbSq = squareToBitboard(curSq);
        }
    }

    return attack;
}

/*
 * Loop through all the squares, and initialize attack bitboards
 * for bishop pieces with all possible relevant occupancies for that
 * square. The traversal of all subsets of a specific occupancy uses
 * the formula a = (a - b) & b. Called the Carry-Rippler method, introduced
 * by Marcel van Kervinck and later by Steffan Westcott.
 */
void initBishopAttacks()
{
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        FancyMagic& curMagic = BISHOP_FANCY_MAGICS[sq];
        curMagic.shift = BISHOP_SHIFT[sq];
        curMagic.magicNumber = BISHOP_MAGIC_NUMBERS[sq];
        curMagic.occupancyMask = BISHOP_OCCUPANCY[sq];

        // Calculate pointer address into BISHOP_ATTACKS_TABLE for sq
        if(sq == A1)
        {
            curMagic.attackTablePointer = BISHOP_ATTACKS_TABLE;
        }
        else
        {
            U64* previousSqPointer = BISHOP_FANCY_MAGICS[sq - 1].attackTablePointer;
            int fancyBitsUsed = 64 - BISHOP_SHIFT[sq - 1];
            int pointerOffset = 1ULL << fancyBitsUsed;
            curMagic.attackTablePointer = previousSqPointer + pointerOffset;
        }

        U64 currentOccupancy { 0ULL };
        do
        {
            U64 curIndex { (currentOccupancy * curMagic.magicNumber) >> curMagic.shift };
            U64 curAttack { calculateBishopAttacks(sq, currentOccupancy) };

            curMagic.attackTablePointer[curIndex] = curAttack;

            currentOccupancy = (currentOccupancy - curMagic.occupancyMask) & curMagic.occupancyMask;
        } while (currentOccupancy);
    }
}

void Attack::initBishopRookAttacks()
{
    initRookAttacks();
    initBishopAttacks();
}