#include "attack.h" // PAWN_ATTACKS, KNIGHT_ATTACKS, KING_ATTACKS, ROOK_FANCY_MAGICS, ROOK_ATTACKS_TABLE, BISHOP_FANCY_MAGICS, BISHOP_ATTACKS_TABLE, calculateRookAttacks, calculateBishopAttacks, Attack::rookAttacks, Attack::bishopAttacks, Attack::queenAttacks, Attack::pawnAttacks, Attack::knightAttacks, Attack::kingAttacks, Attack::isSquareAttacked
#include "bitboard.h" // squareToBitboard
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

TEST_CASE("isSquareAttacked detects a rook attack along an open file, and is blocked by an occupant")
{
    U64 rooks { squareToBitboard(std::to_underlying(LERFSquare::A1)) };
    U64 blocker { squareToBitboard(std::to_underlying(LERFSquare::A4)) };
    U64 occupancy { rooks | blocker };

    CHECK(Attack::isSquareAttacked(LERFSquare::A4, Side::WHITE, occupancy, 0ULL, 0ULL, 0ULL, rooks, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::A5, Side::WHITE, occupancy, 0ULL, 0ULL, 0ULL, rooks, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::B2, Side::WHITE, occupancy, 0ULL, 0ULL, 0ULL, rooks, 0ULL));
}

TEST_CASE("isSquareAttacked detects a bishop attack along an open diagonal, and is blocked by an occupant")
{
    U64 bishops { squareToBitboard(std::to_underlying(LERFSquare::C1)) };
    U64 blocker { squareToBitboard(std::to_underlying(LERFSquare::E3)) };
    U64 occupancy { bishops | blocker };

    CHECK(Attack::isSquareAttacked(LERFSquare::E3, Side::WHITE, occupancy, 0ULL, 0ULL, bishops, 0ULL, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::F4, Side::WHITE, occupancy, 0ULL, 0ULL, bishops, 0ULL, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::D1, Side::WHITE, occupancy, 0ULL, 0ULL, bishops, 0ULL, 0ULL));
}

TEST_CASE("isSquareAttacked detects a queen attack as the union of rook- and bishop-like reach")
{
    U64 queen { squareToBitboard(std::to_underlying(LERFSquare::D1)) };
    U64 occupancy { queen };

    CHECK(Attack::isSquareAttacked(LERFSquare::D8, Side::WHITE, occupancy, 0ULL, 0ULL, queen, queen, 0ULL));
    CHECK(Attack::isSquareAttacked(LERFSquare::H5, Side::WHITE, occupancy, 0ULL, 0ULL, queen, queen, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::E3, Side::WHITE, occupancy, 0ULL, 0ULL, queen, queen, 0ULL));
}

TEST_CASE("isSquareAttacked detects a knight attack")
{
    U64 knights { squareToBitboard(std::to_underlying(LERFSquare::D4)) };

    CHECK(Attack::isSquareAttacked(LERFSquare::E6, Side::WHITE, knights, 0ULL, knights, 0ULL, 0ULL, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::D6, Side::WHITE, knights, 0ULL, knights, 0ULL, 0ULL, 0ULL));
}

TEST_CASE("isSquareAttacked detects a king attack")
{
    U64 king { squareToBitboard(std::to_underlying(LERFSquare::E1)) };

    CHECK(Attack::isSquareAttacked(LERFSquare::E2, Side::WHITE, king, 0ULL, 0ULL, 0ULL, 0ULL, king));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::E3, Side::WHITE, king, 0ULL, 0ULL, 0ULL, 0ULL, king));
}

TEST_CASE("isSquareAttacked detects pawn attacks in the correct direction for each side")
{
    U64 whitePawn { squareToBitboard(std::to_underlying(LERFSquare::D2)) };
    CHECK(Attack::isSquareAttacked(LERFSquare::E3, Side::WHITE, whitePawn, whitePawn, 0ULL, 0ULL, 0ULL, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::E1, Side::WHITE, whitePawn, whitePawn, 0ULL, 0ULL, 0ULL, 0ULL));

    U64 blackPawn { squareToBitboard(std::to_underlying(LERFSquare::D7)) };
    CHECK(Attack::isSquareAttacked(LERFSquare::E6, Side::BLACK, blackPawn, blackPawn, 0ULL, 0ULL, 0ULL, 0ULL));
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::E8, Side::BLACK, blackPawn, blackPawn, 0ULL, 0ULL, 0ULL, 0ULL));
}

TEST_CASE("isSquareAttacked returns true if attacked by any one of multiple attacker types, false if none reach")
{
    U64 knights { squareToBitboard(std::to_underlying(LERFSquare::B1)) };
    U64 rooksAndQueens { squareToBitboard(std::to_underlying(LERFSquare::H4)) };
    U64 occupancy { knights | rooksAndQueens };

    // C3 is reachable by the knight on B1 but not the rook on H4.
    CHECK(Attack::isSquareAttacked(LERFSquare::C3, Side::WHITE, occupancy, 0ULL, knights, 0ULL, rooksAndQueens, 0ULL));
    // D4 is reachable by the rook on H4 but not the knight on B1.
    CHECK(Attack::isSquareAttacked(LERFSquare::D4, Side::WHITE, occupancy, 0ULL, knights, 0ULL, rooksAndQueens, 0ULL));
    // B8 is reachable by neither.
    CHECK_FALSE(Attack::isSquareAttacked(LERFSquare::B8, Side::WHITE, occupancy, 0ULL, knights, 0ULL, rooksAndQueens, 0ULL));
}
