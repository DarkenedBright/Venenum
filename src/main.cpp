#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main()
{
    std::cout << "Venenum - A UCI Chess Engine\n";

    //Read UCI Protocol from standard input
    std::string inputLine{};
    while(true)
    {
        std::string inputLine;
        std::getline(std::cin, inputLine, '\n');
        std::stringstream inputLineStream{inputLine};

        std::vector<std::string> inputArray{};

        std::string inputSegment{};

        while(std::getline(inputLineStream, inputSegment, ' '))
        {
            inputArray.push_back(inputSegment);
        }
    }

    return 0;
}
