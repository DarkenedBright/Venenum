#include "board.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    Board position { fenToBoard(STANDARD_START_FEN) };
    outputBoardToConsole(position);

    position = fenToBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"); //1. e4
    outputBoardToConsole(position);

    position = fenToBoard("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"); //1. ... c5
    outputBoardToConsole(position);

    position = fenToBoard("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"); //2. Nf3
    outputBoardToConsole(position);

    //Read UCI Protocol from standard input
    // std::string inputLine{};
    // while(true)
    // {
    //     std::string inputLine;
    //     std::getline(std::cin, inputLine, '\n');
    //     std::stringstream inputLineStream{inputLine};

    //     std::vector<std::string> inputArray{};

    //     std::string inputSegment{};

    //     while(std::getline(inputLineStream, inputSegment, ' '))
    //     {
    //         inputArray.push_back(inputSegment);
    //     }
    // }

    return 0;
}
