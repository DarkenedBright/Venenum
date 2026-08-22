#include "attack.h" // PAWN_ATTACKS, KNIGHT_ATTACKS, KING_ATTACKS, ROOK_FANCY_MAGICS, ROOK_ATTACKS_TABLE, BISHOP_FANCY_MAGICS, BISHOP_ATTACKS_TABLE, calculateRookAttacks, calculateBishopAttacks
#include "doctest.h" // TEST_CASE, CHECK
#include "types.h" // U64, LERFSquare, Side, FancyMagic

#include <cstddef> // std::size_t
#include <utility> // std::to_underlying

TEST_CASE("knight attacks from corner squares")
{
    CHECK(KNIGHT_ATTACKS[std::to_underlying(LERFSquare::A1)] == 0x20400ULL);
    CHECK(KNIGHT_ATTACKS[std::to_underlying(LERFSquare::H1)] == 0x402000ULL);
    CHECK(KNIGHT_ATTACKS[std::to_underlying(LERFSquare::A8)] == 0x4020000000000ULL);
    CHECK(KNIGHT_ATTACKS[std::to_underlying(LERFSquare::H8)] == 0x20400000000000ULL);
}

TEST_CASE("king attacks from corner squares")
{
    CHECK(KING_ATTACKS[std::to_underlying(LERFSquare::A1)] == 0x302ULL);
    CHECK(KING_ATTACKS[std::to_underlying(LERFSquare::H1)] == 0xC040ULL);
    CHECK(KING_ATTACKS[std::to_underlying(LERFSquare::A8)] == 0x203000000000000ULL);
    CHECK(KING_ATTACKS[std::to_underlying(LERFSquare::H8)] == 0x40C0000000000000ULL);
}

TEST_CASE("pawn attacks are zero from the ranks a pawn can never be on")
{
    // A white pawn can never be on rank 8 (it would have promoted), so its
    // attack table there is defined to be empty rather than wrapping off-board.
    for(int sq { std::to_underlying(LERFSquare::A8) }; sq <= std::to_underlying(LERFSquare::H8); ++sq)
    {
        CHECK(PAWN_ATTACKS[std::to_underlying(Side::WHITE)][sq] == 0ULL);
    }
    // Symmetric case for black on rank 1.
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq <= std::to_underlying(LERFSquare::H1); ++sq)
    {
        CHECK(PAWN_ATTACKS[std::to_underlying(Side::BLACK)][sq] == 0ULL);
    }
}

/*
 * For every square, and every occupancy subset of that square's relevant
 * occupancy mask (enumerated via the same Carry-Rippler trick used by
 * buildRookTables()/buildBishopTables() in attack.h), the fancy-magic
 * table lookup must agree with an independent ray-walk. This validates the
 * multiply-shift-offset indexing itself, catching any index collisions
 * between distinct occupancies on the same square -- a class of bug the
 * ray-walk function can't cause and the table-build code doesn't self-check.
 */
TEST_CASE("rook fancy-magic table matches a naive ray-walk for every occupancy subset")
{
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        std::size_t sqIndex { static_cast<std::size_t>(sq) };
        const FancyMagic& magic { ROOK_FANCY_MAGICS[sqIndex] };

        U64 occupancy { 0ULL };
        do
        {
            U64 index { (occupancy * magic.magicNumber) >> magic.shift };
            U64 tableEntry { ROOK_ATTACKS_TABLE[magic.offset + static_cast<std::size_t>(index)] };
            CHECK(tableEntry == calculateRookAttacks(sq, occupancy));

            occupancy = (occupancy - magic.occupancyMask) & magic.occupancyMask;
        } while(occupancy);
    }
}

TEST_CASE("bishop fancy-magic table matches a naive ray-walk for every occupancy subset")
{
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        std::size_t sqIndex { static_cast<std::size_t>(sq) };
        const FancyMagic& magic { BISHOP_FANCY_MAGICS[sqIndex] };

        U64 occupancy { 0ULL };
        do
        {
            U64 index { (occupancy * magic.magicNumber) >> magic.shift };
            U64 tableEntry { BISHOP_ATTACKS_TABLE[magic.offset + static_cast<std::size_t>(index)] };
            CHECK(tableEntry == calculateBishopAttacks(sq, occupancy));

            occupancy = (occupancy - magic.occupancyMask) & magic.occupancyMask;
        } while(occupancy);
    }
}
