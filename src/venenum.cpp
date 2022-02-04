#include "position.h"
#include "uci.h"

#include <iostream> //std::cout

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    //Initialization of Engine
    Position::initZobristPositionKeys();

    Position position { STANDARD_START_FEN };
    position.print();

    readConsole();

    return 0;
}
