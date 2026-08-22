#include "attack.h" // KNIGHT_ATTACKS, ROOK_FANCY_MAGICS, ROOK_ATTACKS_TABLE, ROOK_ATTACKS_TABLE_SIZE, BISHOP_ATTACKS_TABLE, BISHOP_ATTACKS_TABLE_SIZE
#include "types.h" // U64, LERFSquare, FancyMagic

#include <utility> // std::to_underlying

/*
 * No move generator consumes the fancy-magic attack tables yet, so
 * without a translation unit including attack.h, its consteval table
 * generation would never actually be compiled or checked by a build.
 * These static_asserts force that compilation and double as a permanent
 * compile-time regression guard against a broken magic number or offset
 * calculation, since this repo has no test suite to catch it another way.
 */
namespace
{
    constexpr FancyMagic rookA1Magic { ROOK_FANCY_MAGICS[std::to_underlying(LERFSquare::A1)] };
    constexpr std::size_t rookA1EmptyIndex { static_cast<std::size_t>((0ULL * rookA1Magic.magicNumber) >> rookA1Magic.shift) };
    constexpr U64 rookA1EmptyBoardAttack { ROOK_ATTACKS_TABLE[rookA1Magic.offset + rookA1EmptyIndex] };
}

// Knight attack from E2, per the documented example in attack.h.
static_assert(KNIGHT_ATTACKS[std::to_underlying(LERFSquare::E2)] == 0x28440044ULL);
static_assert(ROOK_ATTACKS_TABLE.size() == ROOK_ATTACKS_TABLE_SIZE);
static_assert(BISHOP_ATTACKS_TABLE.size() == BISHOP_ATTACKS_TABLE_SIZE);
// Rook on A1 with an empty board attacks the whole A-file and 1st rank, minus A1 itself.
static_assert(rookA1EmptyBoardAttack == 0x1010101010101FEULL);
