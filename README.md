# Venenum

Venenum is a chess engine that speaks the [Universal Chess Interface](https://www.chessprogramming.org/UCI) (UCI) protocol, the standard way engines communicate with chess GUIs (Arena, CuteChess, ChessBase, etc.) and command lines.

It is licensed under the GNU General Public License v3.0 — see [LICENSE](LICENSE).

## Requirements

- A C++ compiler with C++20 support (developed against `g++` with `-std=c++2a`)
- `make`

## Building

```sh
make
```

This produces a `Venenum` executable in the repository root. To remove build artifacts:

```sh
make clean
```

On Windows, use the `cleanw`/`cleandepw` targets in place of `clean`/`cleandep`.

## Running

Venenum is a console application that reads UCI commands from standard input. Point a UCI-compatible chess GUI at the built `Venenum` executable, or run it directly and type commands manually:

```sh
./Venenum
```

```
Venenum - A UCI Chess Engine
uci
uciok
```

## Current status

Venenum is under active early development. The UCI handshake (`uci`, `isready`) responds correctly, but most gameplay-relevant commands (`position`, `go`, `setoption`, and others) are recognized but not yet implemented — they currently print a warning instead of acting. Move generation and search are not yet implemented, so Venenum cannot yet play a game end-to-end.

## License

[GNU General Public License v3.0](LICENSE)
