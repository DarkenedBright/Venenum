#include "board.h"

#include <iostream>

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    Board position { fenToBoard(STANDARD_START_FEN) };
    outputBoardToConsole(position);

    return 0;
}
