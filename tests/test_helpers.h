#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "doctest.h" // REQUIRE
#include "position.h" // Position

#include <string_view> // std::string_view

/*
 * Parse fen and REQUIRE it succeeds, returning the resulting Position.
 * Shared by every test file that only cares about a valid FEN's
 * resulting Position, not fromFen()'s error handling itself (see
 * test_position.cpp for those cases).
 */
[[nodiscard]] inline Position parsePosition(std::string_view fen)
{
    auto result { Position::fromFen(fen) };
    REQUIRE(result.has_value());
    return result.value();
}

#endif
