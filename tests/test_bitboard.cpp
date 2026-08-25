#include "bitboard.h" // popcount, setBit, resetBit, squareToBitboard, getLSBIndex
#include "doctest.h" // TEST_CASE, CHECK

TEST_CASE("popcount counts set bits")
{
    CHECK(popcount(0ULL) == 0);
    CHECK(popcount(0xFFFFFFFFFFFFFFFFULL) == 64);
    CHECK(popcount(1ULL) == 1);
    CHECK(popcount(0x8000000000000000ULL) == 1);
    CHECK(popcount(0x5555555555555555ULL) == 32);
}

TEST_CASE("setBit sets exactly the target bit")
{
    for(int sq { 0 }; sq < 64; ++sq)
    {
        U64 bb { setBit(0ULL, sq) };
        CHECK(bb == (1ULL << sq));
    }
}

TEST_CASE("setBit on an already-set bit is a no-op")
{
    U64 bb { setBit(0ULL, 27) };
    CHECK(setBit(bb, 27) == bb);
}

TEST_CASE("resetBit clears exactly the target bit")
{
    U64 allSet { 0xFFFFFFFFFFFFFFFFULL };
    for(int sq { 0 }; sq < 64; ++sq)
    {
        U64 bb { resetBit(allSet, sq) };
        CHECK(bb == (allSet & ~(1ULL << sq)));
    }
}

TEST_CASE("resetBit on an already-clear bit is a no-op")
{
    U64 bb { setBit(0ULL, 5) };
    CHECK(resetBit(bb, 12) == bb);
}

TEST_CASE("setBit and resetBit round-trip to the original bitboard")
{
    U64 bb { 0x123456789ABCDEF0ULL };
    for(int sq { 0 }; sq < 64; ++sq)
    {
        CHECK(resetBit(setBit(bb, sq), sq) == resetBit(bb, sq));
    }
}

TEST_CASE("squareToBitboard matches setBit(0, sq) for every square")
{
    for(int sq { 0 }; sq < 64; ++sq)
    {
        CHECK(squareToBitboard(sq) == setBit(0ULL, sq));
    }
}

TEST_CASE("squareToBitboard corner squares")
{
    CHECK(squareToBitboard(0) == 1ULL); // A1
    CHECK(squareToBitboard(63) == 0x8000000000000000ULL); // H8
}

TEST_CASE("getLSBIndex finds the set bit of a single-bit bitboard")
{
    for(int sq { 0 }; sq < 64; ++sq)
    {
        CHECK(getLSBIndex(squareToBitboard(sq)) == sq);
    }
}

TEST_CASE("getLSBIndex returns the lowest set bit of a multi-bit bitboard")
{
    CHECK(getLSBIndex(0xFFFFFFFFFFFFFFFFULL) == 0);
    CHECK(getLSBIndex(0xF000000000000000ULL) == 60);
    CHECK(getLSBIndex(setBit(setBit(0ULL, 40), 12)) == 12);
    CHECK(getLSBIndex(0x8000000000000001ULL) == 0);
}

TEST_CASE("getLSBIndex composed with resetBit clears exactly the least significant bit")
{
    U64 bb { 0x123456789ABCDEF0ULL };
    while(bb != 0)
    {
        int before { popcount(bb) };
        int lsb { getLSBIndex(bb) };
        bb = resetBit(bb, lsb);
        CHECK(popcount(bb) == before - 1);
    }
}
