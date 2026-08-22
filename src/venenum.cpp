#include "attack.h" //Attack::initBishopRookAttacks()
#include "position.h" //Position::initZobristPositionKeys()
#include "uci.h" //readConsole()

#include <iostream> //std::cout

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    //Initialization of Engine
    Attack::initBishopRookAttacks();
    Position::initZobristPositionKeys();

    readConsole();

    return 0;
}
