#include "doctest.h" // TEST_CASE, CHECK
#include "uci.h" // readConsole

#include <iostream> // std::cin, std::cout
#include <sstream> // std::istringstream, std::ostringstream
#include <streambuf> // std::streambuf
#include <string> // std::string

namespace
{

/*
 * Drive readConsole() end-to-end: feed it commands as if typed at a UCI
 * GUI's stdin, and capture everything it writes to stdout. A trailing
 * "quit" is appended automatically so the call always returns instead
 * of blocking on an exhausted input stream, since readConsole() only
 * ever exits via its "quit" branch. Swapping std::cin/std::cout's
 * stream buffers (rather than the OS-level file descriptors) works
 * because uci.cpp reads through std::cin and -- since its
 * std::println calls were routed through std::cout -- writes through
 * std::cout, so the swap here needs no help from production code.
 */
[[nodiscard]] std::string runUCI(const std::string& commands)
{
    std::istringstream input { commands + "\nquit\n" };
    std::ostringstream output;

    std::streambuf* savedCin { std::cin.rdbuf(input.rdbuf()) };
    std::streambuf* savedCout { std::cout.rdbuf(output.rdbuf()) };

    readConsole();

    std::cin.rdbuf(savedCin);
    std::cout.rdbuf(savedCout);

    return output.str();
}

} // namespace

TEST_CASE("uci responds with uciok")
{
    CHECK(runUCI("uci").find("uciok") != std::string::npos);
}

TEST_CASE("isready responds with readyok")
{
    CHECK(runUCI("isready").find("readyok") != std::string::npos);
}

TEST_CASE("setoption echoes the parsed name and value")
{
    CHECK(runUCI("setoption name Foo value Bar").find("Name: Foo, Value: Bar") != std::string::npos);
}

TEST_CASE("stub commands each respond without crashing")
{
    std::string output { runUCI("debug\nregister\nucinewgame\nstop\nponderhit") };

    CHECK(output.find("'debug' is not implemented") != std::string::npos);
    CHECK(output.find("'register' is not implemented") != std::string::npos);
    CHECK(output.find("'ucinewgame' is not implemented") != std::string::npos);
    CHECK(output.find("'stop' is not implemented") != std::string::npos);
    CHECK(output.find("'ponderhit' is not implemented") != std::string::npos);
}

TEST_CASE("position fen rejects a malformed FEN string")
{
    CHECK(runUCI("position fen not-a-valid-fen").find("Invalid 'position' command") != std::string::npos);
}

TEST_CASE("position fen sets up the given position for go perft")
{
    // Reference count taken from this engine's own perft, already
    // validated against known node counts (see test_perft.cpp) --
    // this test's purpose is to confirm the FEN branch of
    // commandPosition wires correctly into go perft, not to
    // re-validate perft itself.
    std::string output { runUCI("position fen 8/8/8/8/8/8/8/R3K2R w KQkq - 0 1\ngo perft 1") };

    CHECK(output.find("Nodes searched: 26") != std::string::npos);
}

TEST_CASE("position startpos followed by go perft reports the known starting-position move count")
{
    CHECK(runUCI("position startpos\ngo perft 1").find("Nodes searched: 20") != std::string::npos);
}

TEST_CASE("go perft divides the node count by root move")
{
    std::string output { runUCI("position startpos\ngo perft 1") };

    // At depth 1 every root move's own subtree is a single leaf.
    CHECK(output.find("e2e4: 1") != std::string::npos);
    CHECK(output.find("g1f3: 1") != std::string::npos);
}

TEST_CASE("go perft requires a depth of at least 1")
{
    CHECK(runUCI("position startpos\ngo perft 0").find("requires a position and a depth") != std::string::npos);
}

TEST_CASE("position moves applies moves before go perft counts from the resulting position")
{
    // The well-known legal-move count for White after 1.e4 e5.
    CHECK(runUCI("position startpos moves e2e4 e7e5\ngo perft 1").find("Nodes searched: 29") != std::string::npos);
}

TEST_CASE("position moves stops applying at the first illegal or malformed move")
{
    // e2e4 is applied; zz99 is rejected, so processing of this command
    // stops there and the resulting position is the one after 1.e4
    // alone (Black to move, 20 legal replies).
    std::string output { runUCI("position startpos moves e2e4 zz99\ngo perft 1") };

    CHECK(output.find("Illegal or malformed move") != std::string::npos);
    CHECK(output.find("Nodes searched: 20") != std::string::npos);
}
