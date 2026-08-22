# Contributing to Venenum

Thanks for your interest in working on Venenum. This document covers the project layout, build system, and code conventions to keep the codebase consistent.

## Project layout

| File | Purpose |
| --- | --- |
| `src/types.h` | Core enums (`Piece`, `Side`, `Castle`, `Rank`, `File`, `LERFSquare`, `RayDirection`) and the `FancyMagic` struct. |
| `src/bitboard.h` / `src/bitboard.cpp` | Generic bitboard helpers: `popcount`, `setBit`, `resetBit`, `squareToBitboard` (`constexpr`). |
| `src/prng.h` | `PRNG`, a xorshift pseudorandom number generator used for Zobrist key generation. |
| `src/attack.h` / `src/attack.cpp` | Precomputed pawn/knight/king attack tables and fancy-magic-bitboard sliding-piece (bishop/rook) attack tables, generated entirely at compile time by a `consteval` function; `attack.cpp` exists only to force that generation into the build and guard it with `static_assert`s, since nothing consumes the tables yet. |
| `src/position.h` / `src/position.cpp` | The `Position` class: FEN parsing, Zobrist hashing, board printing. |
| `src/uci.h` / `src/uci.cpp` | The UCI protocol command loop and handlers. |
| `src/venenum.cpp` | `main()` — engine startup and initialization. |
| `tests/` | The test suite (`test_<module>.cpp` per tested `src/` module) and the vendored `doctest.h`. |

## Build system

```sh
make               # build a debug binary (-g -O0, ASan/UBSan) at bin/debug/Venenum
make release       # build an optimized binary (-O2 -DNDEBUG) at bin/release/Venenum
make run           # build and run the binary (add BUILD=release for the optimized build)
make clean         # remove build/ and bin/ entirely
make help          # list available targets
```

Object files and generated `.d` dependency files are written to `build/<debug|release>/`, and the compiled binary to `bin/<debug|release>/`; `src/` stays source-only. All of `build/` and `bin/` are git-ignored. The Makefile auto-detects Windows (via `$(OS)`) to suffix the binary with `.exe`; otherwise it assumes a Unix-like shell (Git Bash/MSYS2/WSL on Windows) providing `mkdir -p` and `rm -rf`.

The project builds with a deliberately strict flag set:

```
-std=c++23 -Wall -Weffc++ -Wextra -Wsign-conversion -Werror -pedantic-errors
```

New code must compile warning-free under these flags. In particular, be deliberate about integer types and signedness (`-Wsign-conversion -Werror` will fail the build on implicit signed/unsigned narrowing), and target C++23 language/library features (`-std=c++23`).

## Testing

```sh
make test           # build (debug, with ASan/UBSan) and run the test suite
make test-release   # build (optimized) and run the test suite
```

Tests live under `tests/`, one `test_<module>.cpp` file per tested `src/`
module (e.g. `tests/test_bitboard.cpp` tests `src/bitboard.cpp`), using
[doctest](https://github.com/doctest/doctest) — vendored verbatim at
`tests/doctest.h` (MIT licensed; the project's only third-party dependency,
since there's no package manager). Because `doctest.h`'s macro-generated
types don't satisfy `-Weffc++`, and no vendored third-party header
realistically does, files under `tests/` compile with the same strict flags
as `src/` *except* `-Weffc++`; `src/*.cpp` is unaffected and still builds
under the full flag set described above. Test files link directly against
the engine's own compiled `build/<debug|release>/*.o` objects (everything
except `venenum.cpp`'s `main()`), so tests exercise the exact translation
units the release binary ships rather than a separately-flagged rebuild.

## Code style

- **Brace initialization**: prefer `Type identifier { initializer };` over `=` assignment-style initialization, including for loop counters and locals (e.g. `int count { 0 };`, `U64 attack { 0ULL };`).
- **`this->` qualification**: member access on `this` is written explicitly (`this->pieceBitboards[...]`, `this->sideToMove`) rather than relying on implicit lookup.
- **Include-comment convention**: every `#include` is followed by a comment listing the specific symbols it's needed for, so a reader can see at a glance why each header is included:

  ```cpp
  #include "types.h" // U64, Piece, LERFSquare, File, Rank, Side, Castle
  #include <string_view> // std::string_view, std::string_view::npos
  ```

  Keep this comment up to date when you add or remove usages of a header — if a header is no longer needed for the symbol named in its comment, update or remove the include.
- **Internal linkage**: functions that are implementation details of a single `.cpp` file (not declared in any header) are wrapped in an unnamed `namespace { ... }` rather than marked `static`. See `src/attack.cpp` or `src/uci.cpp` for examples.
- **`enum class`, not plain `enum`**: `Piece`, `Side`, `Castle`, `Rank`, `File`, `LERFSquare`, and `RayDirection` are scoped enums with an explicit underlying type. Square and direction arithmetic that needs the integer value (`sq + dir`, `rank * 8 + file`, array indexing by square) goes through `std::to_underlying(...)` rather than relying on implicit conversion. `Castle` additionally has `operator|`, `operator|=`, and `operator&` overloads so it stays ergonomic as an OR-able bitmask.
- **Const-correctness**: mark member functions `const` when they don't mutate object state.
- **`[[nodiscard]]`**: apply to functions that return a value with no side effects, so callers can't accidentally discard the result.

## Commit messages

Commit messages (from both human contributors and AI agents) follow the
standard seven-rule convention:

- Separate subject from body with a blank line
- Limit the subject line to 50 characters
- Capitalize the subject line
- Do not end the subject line with a period
- Use the imperative mood in the subject line
- Wrap the body at 72 characters
- Use the body to explain what and why vs. how

```
Summarize changes in around 50 characters or less

More detailed explanatory text, if necessary. Wrap it to about 72
characters or so. In some contexts, the first line is treated as the
subject of the commit and the rest of the text as the body. The
blank line separating the summary from the body is critical (unless
you omit the body entirely); various tools like `log`, `shortlog`
and `rebase` can get confused if you run the two together.

Explain the problem that this commit is solving. Focus on why you
are making this change as opposed to how (the code explains that).
Are there side effects or other unintuitive consequences of this
change? Here's the place to explain them.

Further paragraphs come after blank lines.

 - Bullet points are okay, too

 - Typically a hyphen or asterisk is used for the bullet, preceded
   by a single space, with blank lines in between, but conventions
   vary here
```

A body isn't required for a small, self-explanatory change — omit it
rather than padding the commit with restated subject text.

## Contribution workflow

- Keep commits and pull requests scoped to a single logical change; avoid bundling unrelated fixes.
- Match the conventions above rather than introducing new patterns, unless you're proposing a deliberate convention change (open an issue to discuss first).
- Venenum is licensed under GPLv3 (see [LICENSE](LICENSE)); by contributing, you agree your contributions are licensed under the same terms.
