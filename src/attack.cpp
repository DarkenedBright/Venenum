#include "attack.h"
#include "types.h" //U64

/*
 * Return valid if a slide move of a bishop or rook
 * stayed on the board, and did not wrap around the board
 * to an opposite side file or rank.
 */
bool slideIsValid(int from, int to)
{
    int fromFile { from % NUM_FILES };
    int toFile { to % NUM_FILES };
    int fileDistance { fromFile - toFile };
    
    int fromRank { from / NUM_RANKS };
    int toRank { to / NUM_RANKS };
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
        U64 bbSq { 1ULL << curSq };
        while(slideIsValid(prevSq, curSq))
        {
            attack |= bbSq;

            if(occupancy & bbSq) break;

            prevSq = curSq;
            curSq += dir;
            bbSq = (1ULL << curSq);
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
        FancyMagic curMagic = ROOK_ATTACKS[sq];
        curMagic.shift = ROOK_SHIFT[sq];
        curMagic.magicNumber = ROOK_MAGICS[sq];
        curMagic.attackTable = new U64[1ULL << (64 - curMagic.shift)] {};
        curMagic.occupancyMask = ROOK_OCCUPANCY[sq];

        U64 currentOccupancy { 0ULL };
        do
        {
            U64 curIndex { (currentOccupancy * curMagic.magicNumber) >> curMagic.shift };
            U64 curAttack { calculateRookAttacks(sq, currentOccupancy) };

            curMagic.attackTable[curIndex] = curAttack;

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
        U64 bbSq { 1ULL << curSq };
        while(slideIsValid(prevSq, curSq))
        {
            attack |= bbSq;

            if(occupancy & bbSq) break;

            prevSq = curSq;
            curSq += dir;
            bbSq = (1ULL << curSq);
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
        FancyMagic curMagic = BISHOP_ATTACKS[sq];
        curMagic.shift = BISHOP_SHIFT[sq];
        curMagic.magicNumber = BISHOP_MAGICS[sq];
        curMagic.attackTable = new U64[1ULL << (64 - curMagic.shift)] {};
        curMagic.occupancyMask = BISHOP_OCCUPANCY[sq];

        U64 currentOccupancy { 0ULL };
        do
        {
            U64 curIndex { (currentOccupancy * curMagic.magicNumber) >> curMagic.shift };
            U64 curAttack { calculateBishopAttacks(sq, currentOccupancy) };

            curMagic.attackTable[curIndex] = curAttack;

            currentOccupancy = (currentOccupancy - curMagic.occupancyMask) & curMagic.occupancyMask;
        } while (currentOccupancy);
    }
}

void Attack::initBishopRookAttacks()
{
    initRookAttacks();
    initBishopAttacks();
}