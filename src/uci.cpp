#include "uci.h"
#include "position.h"

#include <ios> //std::streamsize
#include <iostream> //cin, cout
#include <limits> //std::numeric_limits<T>::max
#include <string> //std::string
#include <sstream> //std::istringstream

/*
 * uci
 * Tell engine to use the uci (universal chess interface),
 * this will be sent once as a first command after program boot
 * to tell the engine to switch to uci mode.
 * After receiving the uci command the engine must identify itself with the "id" command
 * and send the "option" commands to tell the GUI which engine settings the engine supports if any.
 * After that the engine should send "uciok" to acknowledge the uci mode.
 * If no uciok is sent within a certain time period, the engine task will be killed by the GUI.
 */
void commandUCI()
{
    // TODO call "id" command
    // TODO print "option" commands
    std::cout << "uciok\n";
}

/*
 * debug [ on | off ]
 * switch the debug mode of the engine on and off.
 * In debug mode the engine should send additional infos to the GUI, e.g. with the "info string" command,
 * to help debugging, e.g. the commands that the engine has received etc.
 * This mode should be switched off by default and this command can be sent
 * any time, also when the engine is thinking.
 */
void commandDebug()
{
    std::cout << "WARNING: Command 'debug' is not implemented.\n";
}

/*
 * isready
 * this is used to synchronize the engine with the GUI. When the GUI has sent a command or
 * multiple commands that can take some time to complete,
 * this command can be used to wait for the engine to be ready again or
 * to ping the engine to find out if it is still alive.
 * E.g. this should be sent after setting the path to the tablebases as this can take some time.
 * This command is also required once before the engine is asked to do any search
 * to wait for the engine to finish initializing.
 * This command must always be answered with "readyok" and can be sent also when the engine is calculating
 * in which case the engine should also immediately answer with "readyok" without stopping the search.
 */
void commandIsReady()
{
    std::cout << "readyok\n";
}

/*
 * setoption name <id> [value <x>]
 * this is sent to the engine when the user wants to change the internal parameters
 * of the engine. For the "button" type no value is needed.
 * One string will be sent for each parameter and this will only be sent when the engine is waiting.
 * The name and value of the option in <id> should not be case sensitive and can inlude spaces.
 * The substrings "value" and "name" should be avoided in <id> and <x> to allow unambiguous parsing,
 * for example do not use <name> = "draw value".
 * Here are some strings for the example below:
 *     "setoption name Nullmove value true\n"
 *     "setoption name Selectivity value 3\n"
 *     "setoption name Style value Risky\n"
 *     "setoption name Clear Hash\n"
 *     "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"
 */
void commandSetOption(std::istringstream& uciStringStream)
{
    std::string name {};
    std::string value {};
    std::string uciPart {};

    uciStringStream >> uciPart;
    if(uciPart != "name")
        return;
    
    uciStringStream >> name;

    if(uciStringStream >> uciPart)
    {
        if(uciPart != "value")
            return;
        uciStringStream >> value;
    }
    std::cout << "WARNING: Command 'setoption' is not implemented.\n";
    std::cout << "Name: " << name << ", Value: " << value << '\n';
}

/*
 * register
 * this is the command to try to register an engine or to tell the engine that registration
 * will be done later. This command should always be sent if the engine	has sent "registration error"
 * at program startup.
 * The following tokens are allowed:
 * * later
 *     the user doesn't want to register the engine now.
 * * name <x>
 *     the engine should be registered with the name <x>
 * * code <y>
 *     the engine should be registered with the code <y>
 * Example:
 *     "register later"
 *     "register name Stefan MK code 4359874324"
 */
void commandRegister()
{
    std::cout << "WARNING: Command 'register' is not implemented.\n";
}

/*
 * ucinewgame
 * this is sent to the engine when the next search (started with "position" and "go") will be from
 * a different game. This can be a new game the engine should play or a new game it should analyse but
 * also the next position from a testsuite with positions only.
 * If the GUI hasn't sent a "ucinewgame" before the first "position" command, the engine shouldn't
 * expect any further ucinewgame commands as the GUI is probably not supporting the ucinewgame command.
 * So the engine should not rely on this command even though all new GUIs should support it.
 * As the engine's reaction to "ucinewgame" can take some time the GUI should always send "isready"
 * after "ucinewgame" to wait for the engine to finish its operation.
 */
void commandUCINewGame()
{
    std::cout << "WARNING: Command 'ucinewgame' is not implemented.\n";
}

/*
 * position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
 * set up the position described in fenstring on the internal board and
 * play the moves on the internal chess board.
 * if the game was played  from the start position the string "startpos" will be sent
 * Note: no "new" command is needed. However, if this position is from a different game than
 * the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween.
 */
void commandPosition(std::istringstream& uciStringStream)
{
    std::string uciPart {};
    std::string fenPosition {};
    uciStringStream >> uciPart;

    if(uciPart == "startpos")
    {
        fenPosition = STANDARD_START_FEN;
        uciStringStream >> uciPart;
    }
    else if (uciPart == "fen")
    {
        while(uciStringStream >> uciPart && uciPart != "moves")
        {
            fenPosition += uciPart + ' ';
        }
    }
    else
    {
        return;
    }

    //TODO Set Position Class? to fen position

    if(uciPart != "moves")
        return;
    
    while(uciStringStream >> uciPart)
    {
        //TODO Apply move to position in order
    }

    std::cout << "WARNING: Command 'position' is not implemented.\n";
    std::cout << "FEN: " << fenPosition << '\n';
}

/*
 * go
 * start calculating on the current position set up with the "position" command.
 * There are a number of commands that can follow this command, all will be sent in the same string.
 * If one command is not sent its value should be interpreted as it would not influence the search.
 * * searchmoves <move1> .... <movei>
 *     restrict search to this moves only
 *     Example: After "position startpos" and "go infinite searchmoves e2e4 d2d4"
 *     the engine should only search the two moves e2e4 and d2d4 in the initial position.
 * * ponder
 *     start searching in pondering mode.
 *     Do not exit the search in ponder mode, even if it's mate!
 *     This means that the last move sent in in the position string is the ponder move.
 *     The engine can do what it wants to do, but after a "ponderhit" command
 *     it should execute the suggested move to ponder on. This means that the ponder move sent by
 *     the GUI can be interpreted as a recommendation about which move to ponder. However, if the
 *     engine decides to ponder on a different move, it should not display any mainlines as they are
 *     likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
 *     on the suggested move.
 * * wtime <x>
 *     white has x msec left on the clock
 * * btime <x>
 *     black has x msec left on the clock
 * * winc <x>
 *     white increment per move in mseconds if x > 0
 * * binc <x>
 *     black increment per move in mseconds if x > 0
 * * movestogo <x>
 *     there are x moves to the next time control,
 *     this will only be sent if x > 0,
 *     if you don't get this and get the wtime and btime it's sudden death
 * * depth <x>
 *     search x plies only.
 * * nodes <x>
 *     search x nodes only,
 * * mate <x>
 *     search for a mate in x moves
 * * movetime <x>
 *     search exactly x mseconds
 * * infinite
 *     search until the "stop" command. Do not exit the search without being told so in this mode!
 */
void commandGo(std::istringstream& uciStringStream)
{
    std::string uciPart {};
    uciStringStream >> uciPart;
    std::cout << "WARNING: Command 'go' is not implemented.\n";
}

/*
 * stop
 * stop calculating as soon as possible,
 * don't forget the "bestmove" and possibly the "ponder" token when finishing the search
 */
void commandStop()
{
    std::cout << "WARNING: Command 'stop' is not implemented.\n";
}

/*
 * ponderhit
 * the user has played the expected move. This will be sent if the engine was told to ponder on the same move
 * the user has played. The engine should continue searching but switch from pondering to normal search.
 */
void commandPonderHit()
{
    std::cout << "WARNING: Command 'ponderhit' is not implemented.\n";
}

/*
 * quit
 * quit the program as soon as possible
 */
void commandQuit()
{
    std::cout << "WARNING: Command 'quit' is not implemented.\n";
}

void readConsole()
{
    std::string line {};
    std::string uciPart {};
    while(true)
    {
        std::getline(std::cin >> std::ws, line);
        std::istringstream uciStringStream { line };
        uciStringStream >> uciPart;

        if(uciPart == "uci") commandUCI();
        else if(uciPart == "debug") commandDebug();
        else if(uciPart == "isready") commandIsReady();
        else if(uciPart == "setoption") commandSetOption(uciStringStream);
        else if(uciPart == "register") commandRegister();
        else if(uciPart == "ucinewgame") commandUCINewGame();
        else if(uciPart == "position") commandPosition(uciStringStream);
        else if(uciPart == "go") commandGo(uciStringStream);
        else if(uciPart == "stop") commandStop();
        else if(uciPart == "ponderhit") commandPonderHit();
        else if(uciPart == "quit")
        {
            commandQuit();
            break;
        }
    }
}
