#ifndef ATTACK_H
#define ATTACK_H

#include "bitboard.h" // squareToBitboard()
#include "types.h" //U64, Side, File, Rank, LERFSquare, RayDirection, FancyMagic

#include <array> // std::array
#include <cstddef> // std::size_t
#include <utility> // std::to_underlying

/*
 * Pawn Attack Example: White Pawn attack from E2
 * E2 in LERFSquare is 12. Pawn attack [White] [E2] = 0x280000ULL
 * 0x280000ULL expanded to binary looks like (with X being E2 square):
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0001 0100
 * 0000 X000
 * 0000 0000
 */
inline constexpr U64 PAWN_ATTACKS[std::to_underlying(Side::NUM_SIDES)][std::to_underlying(LERFSquare::NUM_SQUARES)] {
    { //white pawn attacks
        0x200ULL, 0x500ULL, 0xA00ULL, 0x1400ULL, 0x2800ULL, 0x5000ULL, 0xA000ULL, 0x4000ULL,
        0x20000ULL, 0x50000ULL, 0xA0000ULL, 0x140000ULL, 0x280000ULL, 0x500000ULL, 0xA00000ULL, 0x400000ULL,
        0x2000000ULL, 0x5000000ULL, 0xA000000ULL, 0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xA0000000ULL, 0x40000000ULL,
        0x200000000ULL, 0x500000000ULL, 0xA00000000ULL, 0x1400000000ULL, 0x2800000000ULL, 0x5000000000ULL, 0xA000000000ULL, 0x4000000000ULL,
        0x20000000000ULL, 0x50000000000ULL, 0xA0000000000ULL, 0x140000000000ULL, 0x280000000000ULL, 0x500000000000ULL, 0xA00000000000ULL, 0x400000000000ULL,
        0x2000000000000ULL, 0x5000000000000ULL, 0xA000000000000ULL, 0x14000000000000ULL, 0x28000000000000ULL, 0x50000000000000ULL, 0xA0000000000000ULL, 0x40000000000000ULL,
        0x200000000000000ULL, 0x500000000000000ULL, 0xA00000000000000ULL, 0x1400000000000000ULL, 0x2800000000000000ULL, 0x5000000000000000ULL, 0xA000000000000000ULL, 0x4000000000000000ULL,
        0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL
    },
    { //black pawn attacks
        0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
        0x2ULL, 0x5ULL, 0xAULL, 0x14ULL, 0x28ULL, 0x50ULL, 0xA0ULL, 0x40ULL,
        0x200ULL, 0x500ULL, 0xA00ULL, 0x1400ULL, 0x2800ULL, 0x5000ULL, 0xA000ULL, 0x4000ULL,
        0x20000ULL, 0x50000ULL, 0xA0000ULL, 0x140000ULL, 0x280000ULL, 0x500000ULL, 0xA00000ULL, 0x400000ULL,
        0x2000000ULL, 0x5000000ULL, 0xA000000ULL, 0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xA0000000ULL, 0x40000000ULL,
        0x200000000ULL, 0x500000000ULL, 0xA00000000ULL, 0x1400000000ULL, 0x2800000000ULL, 0x5000000000ULL, 0xA000000000ULL, 0x4000000000ULL,
        0x20000000000ULL, 0x50000000000ULL, 0xA0000000000ULL, 0x140000000000ULL, 0x280000000000ULL, 0x500000000000ULL, 0xA00000000000ULL, 0x400000000000ULL,
        0x2000000000000ULL, 0x5000000000000ULL, 0xA000000000000ULL, 0x14000000000000ULL, 0x28000000000000ULL, 0x50000000000000ULL, 0xA0000000000000ULL, 0x40000000000000ULL
    } 
};

/*
 * Knight Attack Example: White Knight attack from E2
 * E2 in LERFSquare is 12. Knight attack [E2] = 0x28440044ULL
 * 0x28440044ULL expanded to binary looks like (with X being E2 square):
 * 
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0001 0100
 * 0010 0010
 * 0000 X000
 * 0010 0010
 */
inline constexpr U64 KNIGHT_ATTACKS[std::to_underlying(LERFSquare::NUM_SQUARES)] { //knight attacks are color agnostic
    0x20400ULL, 0x50800ULL, 0xA1100ULL, 0x142200ULL, 0x284400ULL, 0x508800ULL, 0xA01000ULL, 0x402000ULL,
    0x2040004ULL, 0x5080008ULL, 0xA110011ULL, 0x14220022ULL, 0x28440044ULL, 0x50880088ULL, 0xA0100010ULL, 0x40200020ULL,
    0x204000402ULL, 0x508000805ULL, 0xA1100110AULL, 0x1422002214ULL, 0x2844004428ULL, 0x5088008850ULL, 0xA0100010A0ULL, 0x4020002040ULL,
    0x20400040200ULL, 0x50800080500ULL, 0xA1100110A00ULL, 0x142200221400ULL, 0x284400442800ULL, 0x508800885000ULL, 0xA0100010A000ULL, 0x402000204000ULL,
    0x2040004020000ULL, 0x5080008050000ULL, 0xA1100110A0000ULL, 0x14220022140000ULL, 0x28440044280000ULL, 0x50880088500000ULL, 0xA0100010A00000ULL, 0x40200020400000ULL,
    0x204000402000000ULL, 0x508000805000000ULL, 0xA1100110A000000ULL, 0x1422002214000000ULL, 0x2844004428000000ULL, 0x5088008850000000ULL, 0xA0100010A0000000ULL, 0x4020002040000000ULL,
    0x400040200000000ULL, 0x800080500000000ULL, 0x1100110A00000000ULL, 0x2200221400000000ULL, 0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010A000000000ULL, 0x2000204000000000ULL,
    0x4020000000000ULL, 0x8050000000000ULL, 0x110A0000000000ULL, 0x22140000000000ULL, 0x44280000000000ULL, 0x88500000000000ULL, 0x10A00000000000ULL, 0x20400000000000ULL
};

/*
 * King Attack Example: White King attack from E2
 * E2 in LERFSquare is 12. King attack [E2] = 0x382838ULL
 * 0x382838ULL expanded to binary looks like (with X being E2 square):
 * 
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0001 1100
 * 0001 X100
 * 0001 1100
 */
inline constexpr U64 KING_ATTACKS[std::to_underlying(LERFSquare::NUM_SQUARES)] { //king attacks are color agnostic
    0x302ULL, 0x705ULL, 0xE0AULL, 0x1C14ULL, 0x3828ULL, 0x7050ULL, 0xE0A0ULL, 0xC040ULL, 
    0x30203ULL, 0x70507ULL, 0xE0A0EULL, 0x1C141CULL, 0x382838ULL, 0x705070ULL, 0xE0A0E0ULL, 0xC040C0ULL, 
    0x3020300ULL, 0x7050700ULL, 0xE0A0E00ULL, 0x1C141C00ULL, 0x38283800ULL, 0x70507000ULL, 0xE0A0E000ULL, 0xC040C000ULL, 
    0x302030000ULL, 0x705070000ULL, 0xE0A0E0000ULL, 0x1C141C0000ULL, 0x3828380000ULL, 0x7050700000ULL, 0xE0A0E00000ULL, 0xC040C00000ULL, 
    0x30203000000ULL, 0x70507000000ULL, 0xE0A0E000000ULL, 0x1C141C000000ULL, 0x382838000000ULL, 0x705070000000ULL, 0xE0A0E0000000ULL, 0xC040C0000000ULL, 
    0x3020300000000ULL, 0x7050700000000ULL, 0xE0A0E00000000ULL, 0x1C141C00000000ULL, 0x38283800000000ULL, 0x70507000000000ULL, 0xE0A0E000000000ULL, 0xC040C000000000ULL, 
    0x302030000000000ULL, 0x705070000000000ULL, 0xE0A0E0000000000ULL, 0x1C141C0000000000ULL, 0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL, 
    0x203000000000000ULL, 0x507000000000000ULL, 0xA0E000000000000ULL, 0x141C000000000000ULL, 0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL
};

/*
 * Occupancy is used to denote relevant squares that could potentially
 * "Block" an attack from a bishop further down an attack ray. As such,
 * only the "inner six bits" of a diagonal are considered, since a piece
 * on the outer edge of the board does not "block" any further squares along
 * the attack ray. This reduces the "index" used to look up attacks with magic
 * bitboards.
 * 
 * Bishop Occupancy Example: White Bishop occupancy from E2
 * E2 in LERFSquare is 12. Bishop occupancy [E2] = 0x244280000ULL
 * 0x244280000ULL expanded to binary looks like (with X being E2 square):
 * 
 * 0000 0000
 * 0000 0000
 * 0000 0000
 * 0100 0000
 * 0010 0010
 * 0001 0100
 * 0000 X000
 * 0000 0000
 */
inline constexpr U64 BISHOP_OCCUPANCY[std::to_underlying(LERFSquare::NUM_SQUARES)] {
    0x40201008040200ULL, 0x402010080400ULL, 0x4020100A00ULL, 0x40221400ULL, 0x2442800ULL, 0x204085000ULL, 0x20408102000ULL, 0x2040810204000ULL, 
    0x20100804020000ULL, 0x40201008040000ULL, 0x4020100A0000ULL, 0x4022140000ULL, 0x244280000ULL, 0x20408500000ULL, 0x2040810200000ULL, 0x4081020400000ULL, 
    0x10080402000200ULL, 0x20100804000400ULL, 0x4020100A000A00ULL, 0x402214001400ULL, 0x24428002800ULL, 0x2040850005000ULL, 0x4081020002000ULL, 0x8102040004000ULL, 
    0x8040200020400ULL, 0x10080400040800ULL, 0x20100A000A1000ULL, 0x40221400142200ULL, 0x2442800284400ULL, 0x4085000500800ULL, 0x8102000201000ULL, 0x10204000402000ULL, 
    0x4020002040800ULL, 0x8040004081000ULL, 0x100A000A102000ULL, 0x22140014224000ULL, 0x44280028440200ULL, 0x8500050080400ULL, 0x10200020100800ULL, 0x20400040201000ULL, 
    0x2000204081000ULL, 0x4000408102000ULL, 0xA000A10204000ULL, 0x14001422400000ULL, 0x28002844020000ULL, 0x50005008040200ULL, 0x20002010080400ULL, 0x40004020100800ULL, 
    0x20408102000ULL, 0x40810204000ULL, 0xA1020400000ULL, 0x142240000000ULL, 0x284402000000ULL, 0x500804020000ULL, 0x201008040200ULL, 0x402010080400ULL, 
    0x2040810204000ULL, 0x4081020400000ULL, 0xA102040000000ULL, 0x14224000000000ULL, 0x28440200000000ULL, 0x50080402000000ULL, 0x20100804020000ULL, 0x40201008040200ULL
};

/*
 * Dense and high quality magics taken from Texel UCI Chess Engine.
 * Magics are multiplied with relevant occupancy bits and shifted,
 * creating an index to look up pre-computed attacks for sliding
 * pieces.
 */
inline constexpr U64 BISHOP_MAGIC_NUMBERS[64] = {
    0x0006EFF5367FF600ULL, 0x00345835BA77FF2BULL, 0x00145F68A3F5DAB6ULL, 0x003A1863FB56F21DULL, 0x0012EB6BFE9D93CDULL, 0x000D82827F3420D6ULL, 0x00074BCD9C7FEC97ULL, 0x000034FE99F9FFFFULL, 
    0x0000746F8D6717F6ULL, 0x00003ACB32E1A3F7ULL, 0x0000185DAF1FFB8AULL, 0x00003A1867F17067ULL, 0x0000038EE0CCF92EULL, 0x000002A2B7FF926EULL, 0x000006C9AA93FF14ULL, 0x00000399B5E5BF87ULL, 
    0x00400F342C951FFCULL, 0x0020230579ED8FF0ULL, 0x007B008A0077DBFDULL, 0x001D00010C13FD46ULL, 0x00040022031C1FFBULL, 0x000FA00FD1CBFF79ULL, 0x000400A4BC9AFFDFULL, 0x000200085E9CFFDAULL,
    0x002A14560A3DBFBDULL, 0x000A0A157B9EAFD1ULL, 0x00060600FD002FFAULL, 0x004006000C009010ULL, 0x001A002042008040ULL, 0x001A00600FD1FFC0ULL, 0x000D0ACE50BF3F8DULL, 0x000183A48434EFD1ULL, 
    0x001FBD7670982A0DULL, 0x000FE24301D81A0FULL, 0x0007FBF82F040041ULL, 0x000040C800008200ULL, 0x007FE17018086006ULL, 0x003B7DDF0FFE1EFFULL, 0x001F92F861DF4A0AULL, 0x000FD713AD98A289ULL, 
    0x000FD6AA751E400CULL, 0x0007F2A63AE9600CULL, 0x0003FF7DFE0E3F00ULL, 0x000003FD2704CE04ULL, 0x00007FC421601D40ULL, 0x007FFF5F70900120ULL, 0x003FA66283556403ULL, 0x001FE31969AEC201ULL,
    0x0007FDFC18AC14BBULL, 0x0003FB96FB568A47ULL, 0x000003F72EA4954DULL, 0x00000003F8DC0383ULL, 0x0000007F3A814490ULL, 0x00007DC5C9CF62A6ULL, 0x007F23D3342897ACULL, 0x003FEE36EEE1565CULL, 
    0x0003FF3E99FCCCC7ULL, 0x000003ECFCFAC5FEULL, 0x00000003F97F7453ULL, 0x0000000003F8DC03ULL, 0x000000007EFA8146ULL, 0x0000007ED3E2EF60ULL, 0x00007F47243ADCD6ULL, 0x007FB65AFABFB3B5ULL
};

/*
 * After a bitboard with relevant occupancy information is multiplied by 
 * the bishop square magic number, the resulting 64 bit number is shifted
 * by the square shift number to remove all irrelevant garbage, to produce
 * a 64 - shift bit index number.
 */
inline constexpr int BISHOP_SHIFT[64] = {
    59, 60, 59, 59, 59, 59, 60, 59,
    60, 60, 59, 59, 59, 59, 60, 60,
    60, 60, 57, 57, 57, 57, 60, 60,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    60, 60, 57, 57, 57, 57, 60, 60,
    60, 60, 59, 59, 59, 59, 60, 60,
    59, 60, 59, 59, 59, 59, 60, 59
};

/*
 * Based on bit shift amounts given in BISHOP_SHIFT,
 * use 0x12C0 as the total number of pre-calculated bishop
 * attack bitboards needed to be stored.
 *
 * 4x 9-bit = 4 x 2^9 = 4 x 512 = 2048
 * 12x 7-bit = 12 x 2^7 = 12 x 128 = 1536
 * 28x 5-bit = 28 x 2^5 = 28 x 32 = 896
 * 20x 4-bit = 20 x 2^4 = 20 x 16 = 320
 * Total = 4800 = 0x12C0
 */
inline constexpr std::size_t BISHOP_ATTACKS_TABLE_SIZE { 0x12C0 };

/*
 * Occupancy is used to denote relevant squares that could potentially
 * "Block" an attack from a rook further down an attack ray. As such,
 * only the "inner six bits" of a rank/file are considered, since a piece
 * on the outer edge of the board does not "block" any further squares along
 * the attack ray. This reduces the "index" used to look up attacks with magic
 * bitboards.
 * 
 * Rook Occupancy Example: White Rook occupancy from E2
 * E2 in LERFSquare is 12. Rook occupancy [E2] = 0x10101010106e00ULL
 * 0x10101010106e00ULL expanded to binary looks like (with X being E2 square):
 * 
 * 0000 0000
 * 0000 1000
 * 0000 1000
 * 0000 1000
 * 0000 1000
 * 0000 1000
 * 0111 X110
 * 0000 0000
 */
inline constexpr U64 ROOK_OCCUPANCY[std::to_underlying(LERFSquare::NUM_SQUARES)] {
    0x101010101017EULL, 0x202020202027CULL, 0x404040404047AULL, 0x8080808080876ULL, 0x1010101010106EULL, 0x2020202020205EULL, 0x4040404040403EULL, 0x8080808080807EULL, 
    0x1010101017E00ULL, 0x2020202027C00ULL, 0x4040404047A00ULL, 0x8080808087600ULL, 0x10101010106E00ULL, 0x20202020205E00ULL, 0x40404040403E00ULL, 0x80808080807E00ULL, 
    0x10101017E0100ULL, 0x20202027C0200ULL, 0x40404047A0400ULL, 0x8080808760800ULL, 0x101010106E1000ULL, 0x202020205E2000ULL, 0x404040403E4000ULL, 0x808080807E8000ULL, 
    0x101017E010100ULL, 0x202027C020200ULL, 0x404047A040400ULL, 0x8080876080800ULL, 0x1010106E101000ULL, 0x2020205E202000ULL, 0x4040403E404000ULL, 0x8080807E808000ULL, 
    0x1017E01010100ULL, 0x2027C02020200ULL, 0x4047A04040400ULL, 0x8087608080800ULL, 0x10106E10101000ULL, 0x20205E20202000ULL, 0x40403E40404000ULL, 0x80807E80808000ULL, 
    0x17E0101010100ULL, 0x27C0202020200ULL, 0x47A0404040400ULL, 0x8760808080800ULL, 0x106E1010101000ULL, 0x205E2020202000ULL, 0x403E4040404000ULL, 0x807E8080808000ULL, 
    0x7E010101010100ULL, 0x7C020202020200ULL, 0x7A040404040400ULL, 0x76080808080800ULL, 0x6E101010101000ULL, 0x5E202020202000ULL, 0x3E404040404000ULL, 0x7E808080808000ULL, 
    0x7E01010101010100ULL, 0x7C02020202020200ULL, 0x7A04040404040400ULL, 0x7608080808080800ULL, 0x6E10101010101000ULL, 0x5E20202020202000ULL, 0x3E40404040404000ULL, 0x7E80808080808000ULL
};

/*
 * Dense and high quality magics taken from Texel UCI Chess Engine.
 * Magics are multiplied with relevant occupancy bits and shifted,
 * creating an index to look up pre-computed attacks for sliding
 * pieces.
 */
inline constexpr U64 ROOK_MAGIC_NUMBERS[64] = {
    0x19A80065FF2BFFFFULL, 0x3FD80075FFEBFFFFULL, 0x4010000DF6F6FFFEULL, 0x0050001FAFFAFFFFULL, 0x0050028004FFFFB0ULL, 0x7F600280089FFFF1ULL, 0x7F5000B0029FFFFCULL, 0x5B58004848A7FFFAULL, 
    0x002A90005547FFFFULL, 0x000050007F13FFFFULL, 0x007FA0006013FFFFULL, 0x006A9005656FFFFFULL, 0x007F600F600AFFFFULL, 0x007EC007E6BFFFE2ULL, 0x007EC003EEBFFFFBULL, 0x0071D002382FFFDAULL, 
    0x009F803000E7FFFAULL, 0x00680030008BFFFFULL, 0x00606060004F3FFCULL, 0x001A00600BFF9FFDULL, 0x000D006005FF9FFFULL, 0x0001806003005FFFULL, 0x00000300040BFFFAULL, 0x000192500065FFEAULL,
    0x00FFF112D0006800ULL, 0x007FF037D000C004ULL, 0x003FD062001A3FF8ULL, 0x00087000600E1FFCULL, 0x000FFF0100100804ULL, 0x0007FF0100080402ULL, 0x0003FFE0C0060003ULL, 0x0001FFD53000D300ULL, 
    0x00FFFD3000600061ULL, 0x007FFF7F95900040ULL, 0x003FFF8C00600060ULL, 0x001FFE2587A01860ULL, 0x000FFF3FBF40180CULL, 0x0007FFC73F400C06ULL, 0x0003FF86D2C01405ULL, 0x0001FFFEAA700100ULL, 
    0x00FFFDFDD8005000ULL, 0x007FFF80EBFFB000ULL, 0x003FFFDF603F6000ULL, 0x001FFFE050405000ULL, 0x000FFF400700C00CULL, 0x0007FF6007BF600AULL, 0x0003FFEEBFFEC005ULL, 0x0001FFFDF3FEB001ULL,
    0x00FFFF39FF484A00ULL, 0x007FFF3FFF486300ULL, 0x003FFF99FFAC2E00ULL, 0x001FFF31FF2A6A00ULL, 0x000FFF19FF15B600ULL, 0x0007FFF5FFF28600ULL, 0x0003FFFDDFFBFEE0ULL, 0x0001FFF5F63C96A0ULL, 
    0x00FFFF5DFF65CFB6ULL, 0x007FFFBAFFD1C5AEULL, 0x003FFF71FF6CBCEAULL, 0x001FFFD9FFD4756EULL, 0x000FFFF5FFF338E6ULL, 0x0007FFFDFFFE24F6ULL, 0x0003FFEF27EEBE74ULL, 0x0001FFFF23FF605EULL
};

/*
 * After a bitboard with relevant occupancy information is multiplied by 
 * the rook square magic number, the resulting 64 bit number is shifted
 * by the square shift number to remove all irrelevant garbage, to produce
 * a 64 - shift bit index number.
 */
inline constexpr int ROOK_SHIFT[std::to_underlying(LERFSquare::NUM_SQUARES)] = {
    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 53, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 53, 54, 54, 54, 53,
    54, 55, 55, 55, 55, 55, 54, 54,
    53, 54, 54, 54, 54, 53, 54, 53
};

/*
 * Based on bit shift amounts given in ROOK_SHIFT,
 * use 0x16200 as the total number of pre-calculated rook
 * attack bitboards needed to be stored.
 *
 * 5x 9-bit = 5 x 2^9 = 5 x 512 = 2560
 * 36x 10-bit = 36 x 2^10 = 36 x 1024 = 36864
 * 21x 11-bit = 21 x 2^11 = 21 x 2048 = 43008
 * 2x 12-bit = 2 x 2^12 = 2 x 4096 = 8192
 * Total = 90624 = 0x16200
 */
inline constexpr std::size_t ROOK_ATTACKS_TABLE_SIZE { 0x16200 };

namespace
{

/*
 * Return valid if a slide move of a bishop or rook
 * stayed on the board, and did not wrap around the board
 * to an opposite side file or rank.
 */
[[nodiscard]] constexpr bool slideIsValid(int from, int to)
{
    File fromFile { static_cast<File>(from % std::to_underlying(File::NUM_FILES)) };
    File toFile { static_cast<File>(to % std::to_underlying(File::NUM_FILES)) };
    int fileDistance { std::to_underlying(fromFile) - std::to_underlying(toFile) };

    Rank fromRank { static_cast<Rank>(from / std::to_underlying(Rank::NUM_RANKS)) };
    Rank toRank { static_cast<Rank>(to / std::to_underlying(Rank::NUM_RANKS)) };
    int rankDistance { std::to_underlying(fromRank) - std::to_underlying(toRank) };

    return to >= std::to_underlying(LERFSquare::A1) && to < std::to_underlying(LERFSquare::NUM_SQUARES) && fileDistance > -2 && fileDistance < 2 && rankDistance > -2 && rankDistance < 2;
}

/*
 * Return a bitboard containing all attacked squares on the board from
 * a sliding piece on sq, walking outward along each of the four given
 * ray directions until (and including) the first blocker in
 * occupancy, or the edge of the board. This includes attacked squares
 * with blocker pieces on them; the rook/bishop distinction is purely
 * which four directions are passed in.
 */
[[nodiscard]] constexpr U64 calculateSlidingAttacks(int sq, U64 occupancy, const RayDirection (&directions)[4])
{
    U64 attack { 0ULL };
    for(RayDirection dir: directions)
    {
        int curSq { sq + std::to_underlying(dir) };
        int prevSq { sq };
        while(slideIsValid(prevSq, curSq))
        {
            U64 bbSq { squareToBitboard(curSq) };
            attack |= bbSq;

            if(occupancy & bbSq) break;

            prevSq = curSq;
            curSq += std::to_underlying(dir);
        }
    }

    return attack;
}

/*
 * Return a bitboard containing all attacked
 * squares on the board from a rook on sq with
 * a relevant occupancy. This includes attacked
 * squares with blocker pieces on them.
 */
[[nodiscard]] constexpr U64 calculateRookAttacks(int sq, U64 occupancy)
{
    RayDirection rookDirections[4] { RayDirection::NORTH, RayDirection::EAST, RayDirection::SOUTH, RayDirection::WEST };
    return calculateSlidingAttacks(sq, occupancy, rookDirections);
}

/*
 * Return a bitboard containing all attacked
 * squares on the board from a bishop on sq with
 * a relevant occupancy. This includes attacked
 * squares with blocker pieces on them.
 */
[[nodiscard]] constexpr U64 calculateBishopAttacks(int sq, U64 occupancy)
{
    RayDirection bishopDirections[4] { RayDirection::NORTH_EAST, RayDirection::SOUTH_EAST, RayDirection::SOUTH_WEST, RayDirection::NORTH_WEST };
    return calculateSlidingAttacks(sq, occupancy, bishopDirections);
}

/*
 * Bundles a square's fancy-magic lookup metadata together with the
 * shared attack table it indexes into, so a single consteval function
 * can produce both as one constant expression (see FancyMagic in
 * types.h for why this can't be split into two independently
 * constant-initialized globals).
 */
template<std::size_t TableSize>
struct MagicTables
{
    std::array<FancyMagic, std::to_underlying(LERFSquare::NUM_SQUARES)> magics {};
    std::array<U64, TableSize> table {};
};

/*
 * Loop through all the squares, and calculate attack bitboards for a
 * sliding piece with all possible relevant occupancies for that
 * square, using calculateAttacks (calculateRookAttacks or
 * calculateBishopAttacks) to fill in each entry. The traversal of all
 * subsets of a specific occupancy uses the formula a = (a - b) & b.
 * Called the Carry-Rippler method, introduced by Marcel van Kervinck
 * and later by Steffan Westcott.
 */
template<std::size_t TableSize>
consteval MagicTables<TableSize> buildMagicTables(
    const U64 (&occupancyTable)[std::to_underlying(LERFSquare::NUM_SQUARES)],
    const U64 (&magicNumbers)[std::to_underlying(LERFSquare::NUM_SQUARES)],
    const int (&shiftTable)[std::to_underlying(LERFSquare::NUM_SQUARES)],
    U64 (*calculateAttacks)(int, U64))
{
    MagicTables<TableSize> result {};
    std::size_t runningOffset { 0 };
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        std::size_t sqIndex { static_cast<std::size_t>(sq) };
        FancyMagic& curMagic { result.magics[sqIndex] };
        curMagic.shift = shiftTable[sqIndex];
        curMagic.magicNumber = magicNumbers[sqIndex];
        curMagic.occupancyMask = occupancyTable[sqIndex];
        curMagic.offset = runningOffset;

        U64 currentOccupancy { 0ULL };
        do
        {
            U64 curIndex { (currentOccupancy * curMagic.magicNumber) >> curMagic.shift };
            result.table[curMagic.offset + static_cast<std::size_t>(curIndex)] = calculateAttacks(sq, currentOccupancy);

            currentOccupancy = (currentOccupancy - curMagic.occupancyMask) & curMagic.occupancyMask;
        } while(currentOccupancy);

        U64 fancyBitsUsed { static_cast<U64>(64 - shiftTable[sqIndex]) };
        runningOffset += static_cast<std::size_t>(1ULL << fancyBitsUsed);
    }
    return result;
}

consteval MagicTables<ROOK_ATTACKS_TABLE_SIZE> buildRookTables()
{
    return buildMagicTables<ROOK_ATTACKS_TABLE_SIZE>(ROOK_OCCUPANCY, ROOK_MAGIC_NUMBERS, ROOK_SHIFT, calculateRookAttacks);
}

consteval MagicTables<BISHOP_ATTACKS_TABLE_SIZE> buildBishopTables()
{
    return buildMagicTables<BISHOP_ATTACKS_TABLE_SIZE>(BISHOP_OCCUPANCY, BISHOP_MAGIC_NUMBERS, BISHOP_SHIFT, calculateBishopAttacks);
}

} // namespace

inline constexpr auto ROOK_TABLES { buildRookTables() };
inline constexpr const auto& ROOK_FANCY_MAGICS { ROOK_TABLES.magics };
inline constexpr const auto& ROOK_ATTACKS_TABLE { ROOK_TABLES.table };

inline constexpr auto BISHOP_TABLES { buildBishopTables() };
inline constexpr const auto& BISHOP_FANCY_MAGICS { BISHOP_TABLES.magics };
inline constexpr const auto& BISHOP_ATTACKS_TABLE { BISHOP_TABLES.table };

/*
 * Runtime attack queries. Unlike the tables above, these depend on
 * a board occupancy that's only known at runtime, so they can't be
 * consteval/constexpr themselves -- they just index into the tables
 * that were.
 */
namespace Attack
{

/*
 * Return a bitboard of all squares a rook on sq attacks, given the
 * board's current occupancy (including squares occupied by blockers,
 * whether friend or foe -- callers are responsible for masking those
 * out if they only want empty/enemy-occupied destinations).
 */
[[nodiscard]] inline U64 rookAttacks(LERFSquare sq, U64 occupancy)
{
    const FancyMagic& magic { ROOK_FANCY_MAGICS[static_cast<std::size_t>(std::to_underlying(sq))] };
    U64 index { ((occupancy & magic.occupancyMask) * magic.magicNumber) >> magic.shift };
    return ROOK_ATTACKS_TABLE[magic.offset + static_cast<std::size_t>(index)];
}

/*
 * Return a bitboard of all squares a bishop on sq attacks, given the
 * board's current occupancy. See rookAttacks() for the blocker-masking
 * caveat.
 */
[[nodiscard]] inline U64 bishopAttacks(LERFSquare sq, U64 occupancy)
{
    const FancyMagic& magic { BISHOP_FANCY_MAGICS[static_cast<std::size_t>(std::to_underlying(sq))] };
    U64 index { ((occupancy & magic.occupancyMask) * magic.magicNumber) >> magic.shift };
    return BISHOP_ATTACKS_TABLE[magic.offset + static_cast<std::size_t>(index)];
}

/*
 * Return a bitboard of all squares a queen on sq attacks, given the
 * board's current occupancy. A queen attacks exactly the union of a
 * rook's and a bishop's attacks from the same square.
 */
[[nodiscard]] inline U64 queenAttacks(LERFSquare sq, U64 occupancy)
{
    return rookAttacks(sq, occupancy) | bishopAttacks(sq, occupancy);
}

/*
 * Return a bitboard of all squares a side's pawn on sq attacks.
 * Thin, typed wrapper over PAWN_ATTACKS.
 */
[[nodiscard]] inline U64 pawnAttacks(Side side, LERFSquare sq)
{
    return PAWN_ATTACKS[std::to_underlying(side)][std::to_underlying(sq)];
}

/*
 * Return a bitboard of all squares a knight on sq attacks.
 * Thin, typed wrapper over KNIGHT_ATTACKS.
 */
[[nodiscard]] inline U64 knightAttacks(LERFSquare sq)
{
    return KNIGHT_ATTACKS[std::to_underlying(sq)];
}

/*
 * Return a bitboard of all squares a king on sq attacks.
 * Thin, typed wrapper over KING_ATTACKS.
 */
[[nodiscard]] inline U64 kingAttacks(LERFSquare sq)
{
    return KING_ATTACKS[std::to_underlying(sq)];
}

/*
 * Return whether square is attacked by any piece of bySide, given the
 * board's occupancy and each attacking piece type's bitboard (bishops
 * and queens combined into one parameter, rooks and queens combined
 * into another, since both attack like the sliding piece being probed
 * for). Works by probing outward from square using the attacked
 * piece's own attack pattern: e.g. a knight-attack pattern from square
 * hits an enemy knight exactly when a knight on that enemy square
 * would attack back into square, since the knight-move relation is
 * symmetric (and likewise for king/bishop/rook/queen). Pawns are the
 * one asymmetric case, so the probe uses the opposite side's pawn
 * attack pattern from square to find where an attacking pawn would
 * have to stand.
 */
[[nodiscard]] inline bool isSquareAttacked(LERFSquare square, Side bySide, U64 occupancy, U64 pawns, U64 knights, U64 bishopsAndQueens, U64 rooksAndQueens, U64 king)
{
    Side attackingPawnOrigin { bySide == Side::WHITE ? Side::BLACK : Side::WHITE };
    if(pawnAttacks(attackingPawnOrigin, square) & pawns) return true;
    if(knightAttacks(square) & knights) return true;
    if(kingAttacks(square) & king) return true;
    if(bishopAttacks(square, occupancy) & bishopsAndQueens) return true;
    if(rookAttacks(square, occupancy) & rooksAndQueens) return true;
    return false;
}

} // namespace Attack

#endif