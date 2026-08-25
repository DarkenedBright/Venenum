#include "doctest.h" // TEST_CASE, CHECK
#include "prng.h" // PRNG
#include "types.h" // U64

TEST_CASE("PRNG with the same seed reproduces the same sequence")
{
    PRNG first { 0x1234567890ABCDEFULL };
    PRNG second { 0x1234567890ABCDEFULL };

    for(int i { 0 }; i < 8; ++i)
    {
        CHECK(first.xorShiftRand() == second.xorShiftRand());
    }
}

TEST_CASE("PRNG with different seeds diverges")
{
    PRNG first { 1ULL };
    PRNG second { 2ULL };

    CHECK(first.xorShiftRand() != second.xorShiftRand());
}

TEST_CASE("PRNG consecutive outputs from the same generator differ")
{
    PRNG generator { 0xDEADBEEFCAFEF00DULL };

    U64 previous { generator.xorShiftRand() };
    for(int i { 0 }; i < 8; ++i)
    {
        U64 next { generator.xorShiftRand() };
        CHECK(next != previous);
        previous = next;
    }
}
