#include "board.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

Square rankFileToSquare(Rank rank, File file)
{
    int squareIndex = (8 * rank) + file;
    assert(squareIndex >= A1 && squareIndex <= H8);
    return static_cast<Square>(squareIndex);
}

Rank charRankToEnumRank(char rank)
{
    int rankIndex = rank - '1';
    assert(rankIndex >= RANK_1 && rankIndex <= RANK_8);
    return static_cast<Rank>(rankIndex);
}

File charFileToEnumFile(char file)
{
    int fileIndex = file - 'a';
    assert(fileIndex >= FILE_A && fileIndex <= FILE_H);
    return static_cast<File>(fileIndex);
}

Piece charPieceToEnumPiece(char piece)
{
    switch (piece)
    {
        case 'P':
            return WHITE_PAWN;
        case 'N':
            return WHITE_KNIGHT;
        case 'B':
            return WHITE_BISHOP;
        case 'R':
            return WHITE_ROOK;
        case 'Q':
            return WHITE_QUEEN;
        case 'K':
            return WHITE_KING;
        case 'p':
            return BLACK_PAWN;
        case 'n':
            return BLACK_KNIGHT;
        case 'b':
            return BLACK_BISHOP;
        case 'r':
            return BLACK_ROOK;
        case 'q':
            return BLACK_QUEEN;
        case 'k':
            return BLACK_KING;
        default:
            return EMPTY;
    }
}

char enumPieceToCharPiece(Piece piece)
{
    switch (piece)
    {
        case WHITE_PAWN:
            return 'P';
        case WHITE_KNIGHT:
            return 'N';
        case WHITE_BISHOP:
            return 'B';
        case WHITE_ROOK:
            return 'R';
        case WHITE_QUEEN:
            return 'Q';
        case WHITE_KING:
            return 'K';
        case BLACK_PAWN:
            return 'p';
        case BLACK_KNIGHT:
            return 'n';
        case BLACK_BISHOP:
            return 'b';
        case BLACK_ROOK:
            return 'r';
        case BLACK_QUEEN:
            return 'q';
        case BLACK_KING:
            return 'k';
        case EMPTY:
            return '-';
        default:
            return '?';
    }
}

//TODO: Zobrist Hash on resulting position from FEN string.
Board fenToBoard(std::string fenString)
{
    std::istringstream fenStringStream { fenString };

    Board fenBoard {};
    constexpr std::string_view validPieceChars { "PNBRQKpnbrqk" };
    constexpr std::string_view validCastlingChars { "KQkq-" };
    int sq { A8 };
    char fenChar {};
    int fiftyMoves {};
    int fullMoves {};

    fenStringStream >> std::noskipws;

    // 1. Piece placement (from White's perspective). Each rank is described, starting with rank 8 and ending with rank 1;
    // within each rank, the contents of each square are described from file "a" through file "h". Following the Standard 
    // Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names 
    // (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K"). White pieces are designated using 
    // upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8 
    // (the number of empty squares), and "/" separates ranks.
    while((fenStringStream >> fenChar) && !std::isspace(fenChar))
    {
        assert((std::isdigit(fenChar)) || (fenChar == '/') || (validPieceChars.find(fenChar) != std::string_view::npos));

        if(std::isdigit(fenChar))
        {
            int moveCount { fenChar - '0' };
            assert(moveCount >= 1 && moveCount <= 8);
            sq += moveCount;
        }
        else if(fenChar == '/')
        {
            sq -= 16;
        }
        else
        {
            Piece curPiece = charPieceToEnumPiece(fenChar);
            fenBoard.piecesOnBoard[sq] = curPiece;
            ++sq;
        }
    }

    // 2. Active color. "w" means White moves next, "b" means Black moves next.
    fenStringStream >> fenChar;
    assert((fenChar == 'w') || (fenChar == 'b'));
    fenBoard.sideToMove = (fenChar == 'w') ? WHITE : BLACK;
    fenStringStream >> fenChar;
    assert(std::isspace(fenChar));

    // 3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters: 
    // "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside), 
    // and/or "q" (Black can castle queenside). A move that temporarily prevents castling does not negate this notation.
    while((fenStringStream >> fenChar) && !std::isspace(fenChar))
    {
        assert(validCastlingChars.find(fenChar) != std::string_view::npos);

        switch(fenChar)
        {
            case 'K':
                fenBoard.castlingRights += WHITE_KING_CASTLE;
                break;
            case 'Q':
                fenBoard.castlingRights += WHITE_QUEEN_CASTLE;
                break;
            case 'k':
                fenBoard.castlingRights += BLACK_KING_CASTLE;
                break;
            case 'q':
                fenBoard.castlingRights += BLACK_QUEEN_CASTLE;
                break;
            default:
                // '-' No castle rights
                break;
        }
    }

    // 4. En passant target square in algebraic notation. If there's no en passant target square, this is "-". 
    // If a pawn has just made a two-square move, this is the position "behind" the pawn. This is recorded regardless 
    // of whether there is a pawn in position to make an en passant capture.
    fenStringStream >> fenChar;
    if(fenChar == '-')
    {
        fenBoard.enPassantSquare = NO_SQ;
    }
    else
    {
        File enPassantFile { charFileToEnumFile(fenChar) };

        fenStringStream >> fenChar;
        Rank enPassantRank { charRankToEnumRank(fenChar) };

        assert(enPassantRank >= RANK_1 && enPassantRank <= RANK_8);

        fenBoard.enPassantSquare = rankFileToSquare(enPassantRank, enPassantFile);
    }
    fenStringStream >> fenChar;
    assert(std::isspace(fenChar));

    // 5. Halfmove clock: The number of halfmoves since the last capture or pawn advance, used for the fifty-move rule.
    fenStringStream >> std::skipws;
    fenStringStream >> fiftyMoves;
    assert(fiftyMoves >= 0 && fiftyMoves <= 50);
    fenBoard.fiftyMovesCount = fiftyMoves;

    // 6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.
    fenStringStream >> fullMoves;
    assert(fullMoves >= 1);
    fenBoard.ply = (fullMoves - 1) * 2 + fenBoard.sideToMove;

    return fenBoard;
}

void outputBoardToConsole(Board board)
{
    // 1. Print 8x8 board to console
    for(int rank {RANK_8}; rank >= RANK_1; --rank)
    {
        for(int file {FILE_A}; file <= FILE_H; ++file)
        {
            int sq = rank * 8 + file;
            Piece curPiece = board.piecesOnBoard[sq];
            char pieceChar = enumPieceToCharPiece(curPiece);
            std::cout << pieceChar << ' ';
        }
        std::cout << '\n';
    }

    // 2. Print other state data to console
    std::cout << "EnPassant Square: " << board.enPassantSquare << '\n';
    std::cout << "Castling Rights: " << board.castlingRights << '\n';
    std::cout << "Fifty Moves Count: " << board.fiftyMovesCount << '\n';
    std::cout << "Ply: " << board.ply << '\n';
    std::cout << "Position ID: " << board.positionIdentity << '\n';
    std::cout << "Side to Move: " << board.sideToMove << '\n';
}
