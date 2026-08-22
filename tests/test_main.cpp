#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h" // doctest::Context, DOCTEST_CONFIG_IMPLEMENT

#include "position.h" // Position::initZobristPositionKeys

int main(int argc, char** argv)
{
    Position::initZobristPositionKeys();

    doctest::Context context {};
    context.applyCommandLine(argc, argv);
    return context.run();
}
