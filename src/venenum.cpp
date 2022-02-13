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

    Position position2 { "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2" };
    position2.print();

    readConsole();

    return 0;
}
