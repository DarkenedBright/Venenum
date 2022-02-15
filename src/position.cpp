#include "bitboard.h" // squareToBitboard()
#include "position.h"
#include "prng.h" // PRNG
#include "types.h" // U64, Piece, LERFSquare, File, Rank, Side, Castle

#include <cassert> //assert()
#include <cctype> // std::isspace(), std::isdigit()
#include <ios> // std::skipws, std::noskipws
#include <iostream> // std::cout
#include <sstream> // std::istringstream
#include <string> // std::string, std::string::npos, std::size_t
#include <string_view> // std::string_view, std::string_view::npos

/* 
 * Use Zobrist Hashing
 * Credit: Albert L. Zobrist, The University of Wisconsin
 * https://research.cs.wisc.edu/techreports/1970/TR88.pdf
 */
void Position::initZobristPositionKeys()
{
    PRNG randGen { POSITION_ZOBRIST_SEED };

    // U64 pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        for(int piece { EMPTY }; piece < NUM_PIECES; ++piece)
        {
            pieceSquareKeys[sq][piece] = randGen.xorShiftRand();
        }
    }

    // U64 sideToMoveKey;
    sideToMoveKey = randGen.xorShiftRand();

    // U64 castlingRightKeys[NUM_CASTLE_STATES];
    for(int castle { 0 }; castle < NUM_CASTLE_STATES; ++castle)
    {
        castlingRightKeys[castle] = randGen.xorShiftRand();
    }

    // U64 enPassantFileKeys[NUM_FILES];
    for(int file { FILE_A }; file < NUM_FILES; ++file)
    {
        enPassantFileKeys[file] = randGen.xorShiftRand();
    }
}

Position::Position(const std::string& fenString)
{
    std::istringstream fenStringStream { fenString };

    constexpr std::string_view validPieceChars { "PNBRQKpnbrqk" };
    constexpr std::string_view validCastlingChars { "KQkq-" };
    int sq { A8 };
    U64 sqBB { squareToBitboard(sq) };
    char fenChar {};
    int fiftyMoves {};
    int fullMoves {};

    fenStringStream >> std::noskipws;

    /* 
     * 1. Piece placement (from White's perspective). Each rank is described, starting with rank 8 and ending with rank 1;
     * within each rank, the contents of each square are described from file "a" through file "h". Following the Standard 
     * Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names 
     * (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K"). White pieces are designated using 
     * upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8 
     * (the number of empty squares), and "/" separates ranks.
     */
    while((fenStringStream >> fenChar) && !std::isspace(fenChar))
    {
        assert((std::isdigit(fenChar)) || (fenChar == '/') || (validPieceChars.find(fenChar) != std::string_view::npos));

        if(std::isdigit(fenChar))
        {
            // Move along rank by given number of empty squares
            int moveCount { fenChar - '0' };
            assert(moveCount >= 1 && moveCount <= 8);
            sq += moveCount;
        }
        else if(fenChar == '/')
        {
            // Move to the next rank towards white
            sq -= 16;
        }
        else
        {
            // Get the Piece enum from character in FEN
            std::size_t pieceIndex = pieceToChar.find(fenChar);
            assert(pieceIndex != std::string::npos);
            assert(pieceIndex < NUM_PIECES);
            Piece curPiece { pieceIndex };

            // Update Piece Bitboards
            this->pieceBitboards[curPiece] |= sqBB;
            ++sq;
        }
        sqBB = squareToBitboard(sq);
    }
    // Update Color Bitboards and Occupancy Bitboard
    this->pieceBitboards[WHITE_ALL] = ( this->pieceBitboards[WHITE_PAWN] | this->pieceBitboards[WHITE_KNIGHT] | 
                                        this->pieceBitboards[WHITE_BISHOP] | this->pieceBitboards[WHITE_ROOK] | 
                                        this->pieceBitboards[WHITE_QUEEN] | this->pieceBitboards[WHITE_KING] );
    this->pieceBitboards[BLACK_ALL] = ( this->pieceBitboards[BLACK_PAWN] | this->pieceBitboards[BLACK_KNIGHT] | 
                                        this->pieceBitboards[BLACK_BISHOP] | this->pieceBitboards[BLACK_ROOK] | 
                                        this->pieceBitboards[BLACK_QUEEN] | this->pieceBitboards[BLACK_KING] );
    this->pieceBitboards[ALL_PIECES] = ( this->pieceBitboards[WHITE_ALL] | this->pieceBitboards[BLACK_ALL] );
    this->pieceBitboards[EMPTY] = ~pieceBitboards[ALL_PIECES];

    // 2. Active color. "w" means White moves next, "b" means Black moves next.
    fenStringStream >> fenChar;
    assert((fenChar == 'w') || (fenChar == 'b'));
    this->sideToMove = (fenChar == 'w') ? WHITE : BLACK;
    fenStringStream >> fenChar;
    assert(std::isspace(fenChar));

    /* 
     * 3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters: 
     * "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside), 
     * and/or "q" (Black can castle queenside). A move that temporarily prevents castling does not negate this notation.
     */
    while((fenStringStream >> fenChar) && !std::isspace(fenChar))
    {
        assert(validCastlingChars.find(fenChar) != std::string_view::npos);

        switch(fenChar)
        {
            case 'K':
                this->castlingRights += WHITE_KING_CASTLE;
                break;
            case 'Q':
                this->castlingRights += WHITE_QUEEN_CASTLE;
                break;
            case 'k':
                this->castlingRights += BLACK_KING_CASTLE;
                break;
            case 'q':
                this->castlingRights += BLACK_QUEEN_CASTLE;
                break;
            default:
                // '-' No castle rights
                break;
        }
    }

    /* 
     * 4. En passant target square in algebraic notation. If there's no en passant target square, this is "-". 
     * If a pawn has just made a two-square move, this is the position "behind" the pawn. This is recorded regardless 
     * of whether there is a pawn in position to make an en passant capture.
     */
    fenStringStream >> fenChar;
    if(fenChar == '-')
    {
        this->enPassantSquare = NO_SQ;
    }
    else
    {
        size_t fileIndex = fileToChar.find(fenChar);
        assert(fileIndex != std::string::npos);
        assert(fileIndex < NUM_FILES);
        File enPassantFile { fileIndex };

        fenStringStream >> fenChar;

        size_t rankIndex = rankToChar.find(fenChar);
        assert(rankIndex != std::string::npos);
        assert(rankIndex < NUM_RANKS);
        Rank enPassantRank { rankIndex };

        int epSq { enPassantRank * 8 + enPassantFile };
        assert(epSq >= A1 && epSq <= H8);
        this->enPassantSquare = static_cast<LERFSquare>(epSq);
    }
    fenStringStream >> fenChar;
    assert(std::isspace(fenChar));

    // 5. Halfmove clock: The number of halfmoves since the last capture or pawn advance, used for the fifty-move rule.
    fenStringStream >> std::skipws;
    fenStringStream >> fiftyMoves;
    assert(fiftyMoves >= 0 && fiftyMoves <= 50);
    this->fiftyMovesCount = fiftyMoves;

    // 6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.
    fenStringStream >> fullMoves;
    assert(fullMoves >= 1);
    this->ply = (fullMoves - 1) * 2 + this->sideToMove;

    // 7. Compute position hash via Zobrist hashing.
    this->positionIdentity = this->calculatePositionHash();
}

U64 Position::calculatePositionHash()
{
    U64 hash { 0 };

    //Handle piece square keys including empty
    U64 sqBB {};
    for(int sq { A1 }; sq < NUM_SQUARES; ++sq)
    {
        sqBB = squareToBitboard(sq);
        // Loop through piece types
        for(int pieceType { EMPTY }; pieceType < NUM_PIECES; ++pieceType)
        {
            if(this->pieceBitboards[pieceType] & sqBB)
            {
                hash ^= this->pieceSquareKeys[sq][pieceType];
                break;
            }
        }
    }

    //Handle side to move
    if(this->sideToMove == BLACK)
    {
        hash ^= this->sideToMoveKey;
    }

    //Handle castling rights
    hash ^= this->castlingRightKeys[this->castlingRights];

    //Handle enPassant File
    if(this->enPassantSquare != NO_SQ)
    {
        int file = this->enPassantSquare % 8;
        hash ^= this->enPassantFileKeys[file];
    }

    return hash;
}

void Position::print()
{
    // 1. Print 8x8 board to console
    U64 sqBB {};
    char pieceChar {};
    for(int rank {RANK_8}; rank >= RANK_1; --rank)
    {
        for(int file {FILE_A}; file <= FILE_H; ++file)
        {
            sqBB = squareToBitboard(rank * 8 + file);

            for(std::size_t pieceType { EMPTY }; pieceType < NUM_PIECES; ++pieceType)
            {
                if(this->pieceBitboards[pieceType] & sqBB)
                {
                    pieceChar = pieceToChar[pieceType];
                    std::cout << pieceChar << ' ';
                    break;
                }
            }
        }
        std::cout << '\n';
    }

    // 2. Print other state data to console
    std::cout << "\nEnPassant Square: " << this->enPassantSquare << '\n';
    std::cout << "Castling Rights: " << this->castlingRights << '\n';
    std::cout << "Fifty Moves Count: " << this->fiftyMovesCount << '\n';
    std::cout << "Ply: " << this->ply << '\n';
    std::cout << "Position ID: " << this->positionIdentity << '\n';
    std::cout << "Side to Move: " << this->sideToMove << '\n';
}
