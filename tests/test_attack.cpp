#include "attack.h" // PAWN_ATTACKS, KNIGHT_ATTACKS, KING_ATTACKS, ROOK_FANCY_MAGICS, ROOK_ATTACKS_TABLE, BISHOP_FANCY_MAGICS, BISHOP_ATTACKS_TABLE, calculateRookAttacks, calculateBishopAttacks, Attack::rookAttacks, Attack::bishopAttacks, Attack::queenAttacks, Attack::pawnAttacks, Attack::knightAttacks, Attack::kingAttacks
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
TEST_CASE("Attack::rookAttacks matches a naive ray-walk for every occupancy subset")
{
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        std::size_t sqIndex { static_cast<std::size_t>(sq) };
        LERFSquare square { static_cast<LERFSquare>(sq) };
        const FancyMagic& magic { ROOK_FANCY_MAGICS[sqIndex] };

        U64 occupancy { 0ULL };
        do
        {
            CHECK(Attack::rookAttacks(square, occupancy) == calculateRookAttacks(sq, occupancy));

            occupancy = (occupancy - magic.occupancyMask) & magic.occupancyMask;
        } while(occupancy);
    }
}

TEST_CASE("Attack::bishopAttacks matches a naive ray-walk for every occupancy subset")
{
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        std::size_t sqIndex { static_cast<std::size_t>(sq) };
        LERFSquare square { static_cast<LERFSquare>(sq) };
        const FancyMagic& magic { BISHOP_FANCY_MAGICS[sqIndex] };

        U64 occupancy { 0ULL };
        do
        {
            CHECK(Attack::bishopAttacks(square, occupancy) == calculateBishopAttacks(sq, occupancy));

            occupancy = (occupancy - magic.occupancyMask) & magic.occupancyMask;
        } while(occupancy);
    }
}

TEST_CASE("Attack::queenAttacks is the union of rook and bishop attacks from the same square")
{
    U64 occupancy { 0x0000240000810000ULL };
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        LERFSquare square { static_cast<LERFSquare>(sq) };
        CHECK(Attack::queenAttacks(square, occupancy) == (Attack::rookAttacks(square, occupancy) | Attack::bishopAttacks(square, occupancy)));
    }
}

TEST_CASE("Attack::pawnAttacks matches PAWN_ATTACKS")
{
    CHECK(Attack::pawnAttacks(Side::WHITE, LERFSquare::E2) == PAWN_ATTACKS[std::to_underlying(Side::WHITE)][std::to_underlying(LERFSquare::E2)]);
    CHECK(Attack::pawnAttacks(Side::BLACK, LERFSquare::E7) == PAWN_ATTACKS[std::to_underlying(Side::BLACK)][std::to_underlying(LERFSquare::E7)]);
}

TEST_CASE("Attack::knightAttacks matches KNIGHT_ATTACKS at corner squares")
{
    CHECK(Attack::knightAttacks(LERFSquare::A1) == 0x20400ULL);
    CHECK(Attack::knightAttacks(LERFSquare::H1) == 0x402000ULL);
    CHECK(Attack::knightAttacks(LERFSquare::A8) == 0x4020000000000ULL);
    CHECK(Attack::knightAttacks(LERFSquare::H8) == 0x20400000000000ULL);
}

TEST_CASE("Attack::kingAttacks matches KING_ATTACKS at corner squares")
{
    CHECK(Attack::kingAttacks(LERFSquare::A1) == 0x302ULL);
    CHECK(Attack::kingAttacks(LERFSquare::H1) == 0xC040ULL);
    CHECK(Attack::kingAttacks(LERFSquare::A8) == 0x203000000000000ULL);
    CHECK(Attack::kingAttacks(LERFSquare::H8) == 0x40C0000000000000ULL);
}
