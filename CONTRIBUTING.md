# Contributing to Venenum

Thanks for your interest in working on Venenum. This document covers the project layout, build system, and code conventions to keep the codebase consistent.

## Project layout

| File | Purpose |
| --- | --- |
| `src/types.h` | Core enums (`Piece`, `Side`, `Castle`, `Rank`, `File`, `LERFSquare`, `RayDirection`) and the `FancyMagic` struct. |
| `src/bitboard.h` / `src/bitboard.cpp` | Generic bitboard helpers: `popcount`, `setBit`, `resetBit`, `squareToBitboard`. |
| `src/prng.h` | `PRNG`, a xorshift pseudorandom number generator used for Zobrist key generation. |
| `src/attack.h` / `src/attack.cpp` | Precomputed pawn/knight/king attack tables and fancy-magic-bitboard sliding-piece (bishop/rook) attack generation. |
| `src/position.h` / `src/position.cpp` | The `Position` class: FEN parsing, Zobrist hashing, board printing. |
| `src/uci.h` / `src/uci.cpp` | The UCI protocol command loop and handlers. |
| `src/venenum.cpp` | `main()` — engine startup and initialization. |

## Build system

```sh
make          # build the Venenum executable
make clean    # remove build artifacts (Unix)
make cleandep # remove only generated .d dependency files (Unix)
make cleanw / cleandepw  # Windows equivalents
```

Object files are written alongside their sources in `src/`; the compiled binary and generated `.d`/`.o` files are git-ignored.

The project builds with a deliberately strict flag set:

```
-std=c++2a -Wall -Weffc++ -Wextra -Wsign-conversion -Werror -pedantic-errors
```

New code must compile warning-free under these flags. In particular, be deliberate about integer types and signedness (`-Wsign-conversion -Werror` will fail the build on implicit signed/unsigned narrowing), and target C++20 language/library features (`-std=c++2a`).

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
- **`enum`, not `enum class`**: `Piece`, `Side`, `Castle`, `Rank`, `File`, `LERFSquare`, and `RayDirection` are deliberately plain (unscoped) enums, not `enum class`. Square and direction arithmetic (`sq + dir`, `rank * 8 + file`, array indexing by square) relies on implicit conversion to `int`, which is standard practice in bitboard-based chess engines. Don't convert these to scoped enums.
- **Const-correctness**: mark member functions `const` when they don't mutate object state.
- **`[[nodiscard]]`**: apply to functions that return a value with no side effects, so callers can't accidentally discard the result.

## Contribution workflow

- Keep commits and pull requests scoped to a single logical change; avoid bundling unrelated fixes.
- Match the conventions above rather than introducing new patterns, unless you're proposing a deliberate convention change (open an issue to discuss first).
- Venenum is licensed under GPLv3 (see [LICENSE](LICENSE)); by contributing, you agree your contributions are licensed under the same terms.
