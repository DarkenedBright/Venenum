#include "attack.h" //Attack::init()
#include "position.h" //Position::init()
#include "uci.h" //readConsole()

#include <iostream> //std::cout

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    //Initialization of Engine
    Attack::initBishopRookAttacks();
    Position::initZobristPositionKeys();

    Position position { STANDARD_START_FEN };
    position.print();

    readConsole();

    return 0;
}
