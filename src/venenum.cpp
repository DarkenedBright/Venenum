#include "attack.h" //Attack::initBishopRookAttacks()
#include "position.h" //Position::initZobristPositionKeys()
#include "uci.h" //readConsole()

#include <print> //std::println

int main()
{
    std::println("Venenum - A UCI Chess Engine");

    //Initialization of Engine
    Attack::initBishopRookAttacks();
    Position::initZobristPositionKeys();

    readConsole();

    return 0;
}
