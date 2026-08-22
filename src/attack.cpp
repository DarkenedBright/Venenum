#include "attack.h" // ROOK_FANCY_MAGICS, ROOK_ATTACKS_TABLE, BISHOP_FANCY_MAGICS, BISHOP_ATTACKS_TABLE
#include "types.h" // U64, LERFSquare, FancyMagic

#include <cstddef> // std::size_t
#include <utility> // std::to_underlying

/*
 * No move generator consumes the fancy-magic attack tables yet, so without a
 * translation unit including attack.h, its consteval table generation would
 * never actually be compiled by a plain `make` build (only by `make test`).
 * This file exists to force that compilation. The two static_asserts below
 * are minimal compile-time sanity checks on the generated tables; exhaustive
 * validation of every square and occupancy combination against an
 * independent ray-walk lives in tests/test_attack.cpp.
 */
namespace
{
    constexpr FancyMagic rookA1Magic { ROOK_FANCY_MAGICS[std::to_underlying(LERFSquare::A1)] };
    constexpr std::size_t rookA1EmptyIndex { static_cast<std::size_t>((0ULL * rookA1Magic.magicNumber) >> rookA1Magic.shift) };
    constexpr U64 rookA1EmptyBoardAttack { ROOK_ATTACKS_TABLE[rookA1Magic.offset + rookA1EmptyIndex] };

    constexpr FancyMagic bishopA1Magic { BISHOP_FANCY_MAGICS[std::to_underlying(LERFSquare::A1)] };
    constexpr std::size_t bishopA1EmptyIndex { static_cast<std::size_t>((0ULL * bishopA1Magic.magicNumber) >> bishopA1Magic.shift) };
    constexpr U64 bishopA1EmptyBoardAttack { BISHOP_ATTACKS_TABLE[bishopA1Magic.offset + bishopA1EmptyIndex] };
}

// Rook on A1 with an empty board attacks the whole A-file and 1st rank, minus A1 itself.
static_assert(rookA1EmptyBoardAttack == 0x1010101010101FEULL);
// Bishop on A1 with an empty board attacks the whole A1-H8 diagonal, minus A1 itself.
static_assert(bishopA1EmptyBoardAttack == 0x8040201008040200ULL);
