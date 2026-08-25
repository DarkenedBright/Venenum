#include "bitboard.h" // squareToBitboard()
#include "move.h" // Move, MoveFlag
#include "position.h"
#include "prng.h" // PRNG
#include "types.h" // U64, Piece, LERFSquare, File, Rank, Side, Castle, RayDirection, sidedPiece, allPiecesOf, fileOf, rankOf

#include <cctype> // std::isspace(), std::isdigit()
#include <cstddef> // std::size_t
#include <expected> // std::expected, std::unexpected
#include <ios> // std::skipws, std::noskipws
#include <print> // std::print, std::println
#include <sstream> // std::istringstream
#include <string> // std::string
#include <string_view> // std::string_view, std::string_view::npos
#include <utility> // std::to_underlying

namespace
{

[[nodiscard]] Side pieceSide(Piece piece)
{
    return std::to_underlying(piece) <= std::to_underlying(Piece::WHITE_KING) ? Side::WHITE : Side::BLACK;
}

/*
 * Castling rights lost when a king or rook moves off (or is captured
 * on) its home square. Applied to both the "from" and "to" square of
 * every move: a king move clears both rights on that side via its
 * home square, and a rook being captured on its home square clears
 * that side's right without needing special-case capture handling.
 */
[[nodiscard]] Castle castleRightsLostAt(LERFSquare square)
{
    switch(square)
    {
        case LERFSquare::E1: return Castle::WHITE_KING_CASTLE | Castle::WHITE_QUEEN_CASTLE;
        case LERFSquare::A1: return Castle::WHITE_QUEEN_CASTLE;
        case LERFSquare::H1: return Castle::WHITE_KING_CASTLE;
        case LERFSquare::E8: return Castle::BLACK_KING_CASTLE | Castle::BLACK_QUEEN_CASTLE;
        case LERFSquare::A8: return Castle::BLACK_QUEEN_CASTLE;
        case LERFSquare::H8: return Castle::BLACK_KING_CASTLE;
        default: return static_cast<Castle>(0);
    }
}

[[nodiscard]] Castle clearCastleRights(Castle rights, Castle toClear)
{
    return static_cast<Castle>(std::to_underlying(rights) & ~std::to_underlying(toClear));
}

/*
 * The rook's from/to squares for a castling move, indexed by which
 * side is castling and which flag (king- or queen-side) it is.
 */
[[nodiscard]] LERFSquare castleRookFrom(Side side, MoveFlag flag)
{
    if(flag == MoveFlag::KING_CASTLE)
        return side == Side::WHITE ? LERFSquare::H1 : LERFSquare::H8;
    return side == Side::WHITE ? LERFSquare::A1 : LERFSquare::A8;
}

[[nodiscard]] LERFSquare castleRookTo(Side side, MoveFlag flag)
{
    if(flag == MoveFlag::KING_CASTLE)
        return side == Side::WHITE ? LERFSquare::F1 : LERFSquare::F8;
    return side == Side::WHITE ? LERFSquare::D1 : LERFSquare::D8;
}

/*
 * The pawn square captured by an en passant move: directly behind
 * the "to" square from the mover's perspective.
 */
[[nodiscard]] LERFSquare enPassantCapturedPawnSquare(Side movingSide, LERFSquare to)
{
    int offset { movingSide == Side::WHITE ? std::to_underlying(RayDirection::SOUTH) : std::to_underlying(RayDirection::NORTH) };
    return static_cast<LERFSquare>(std::to_underlying(to) + offset);
}

} // namespace

/*
 * Use Zobrist Hashing
 * Credit: Albert L. Zobrist, The University of Wisconsin
 * https://research.cs.wisc.edu/techreports/1970/TR88.pdf
 */
void Position::initZobristPositionKeys()
{
    PRNG randGen { POSITION_ZOBRIST_SEED };

    // U64 pieceSquareKeys[NUM_SQUARES][NUM_PIECES];
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        for(int piece { std::to_underlying(Piece::EMPTY) }; piece < std::to_underlying(Piece::NUM_PIECES); ++piece)
        {
            pieceSquareKeys[sq][piece] = randGen.xorShiftRand();
        }
    }

    // U64 sideToMoveKey;
    sideToMoveKey = randGen.xorShiftRand();

    // U64 castlingRightKeys[NUM_CASTLE_STATES];
    for(int castle { 0 }; castle < std::to_underlying(Castle::NUM_CASTLE_STATES); ++castle)
    {
        castlingRightKeys[castle] = randGen.xorShiftRand();
    }

    // U64 enPassantFileKeys[NUM_FILES];
    for(int file { std::to_underlying(File::FILE_A) }; file < std::to_underlying(File::NUM_FILES); ++file)
    {
        enPassantFileKeys[file] = randGen.xorShiftRand();
    }
}

std::expected<Position, FenParseError> Position::fromFen(std::string_view fenString)
{
    Position position {};
    std::istringstream fenStringStream { std::string(fenString) };

    constexpr std::string_view validPieceChars { "PNBRQKpnbrqk" };
    constexpr std::string_view validCastlingChars { "KQkq-" };
    int sq { std::to_underlying(LERFSquare::A8) };
    char fenChar {};
    int fiftyMoves {};
    int fullMoves {};
    int rankSquareCount { 0 };
    int rankCount { 0 };

    fenStringStream >> std::noskipws;

    /*
     * 1. Piece placement (from White's perspective). Each rank is described, starting with rank 8 and ending with rank 1;
     * within each rank, the contents of each square are described from file "a" through file "h". Following the Standard
     * Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names
     * (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K"). White pieces are designated using
     * upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8
     * (the number of empty squares), and "/" separates ranks.
     */
    while((fenStringStream >> fenChar) && !std::isspace(static_cast<unsigned char>(fenChar)))
    {
        if(!std::isdigit(static_cast<unsigned char>(fenChar)) && fenChar != '/' && validPieceChars.find(fenChar) == std::string_view::npos)
            return std::unexpected(FenParseError::InvalidPiecePlacementChar);

        if(std::isdigit(static_cast<unsigned char>(fenChar)))
        {
            // Move along rank by given number of empty squares
            int moveCount { fenChar - '0' };
            if(moveCount < 1 || moveCount > 8)
                return std::unexpected(FenParseError::InvalidEmptySquareCount);
            if(rankSquareCount + moveCount > 8)
                return std::unexpected(FenParseError::InvalidPiecePlacementRankLength);
            sq += moveCount;
            rankSquareCount += moveCount;
        }
        else if(fenChar == '/')
        {
            // A rank must be fully described before moving to the next one.
            if(rankSquareCount != 8)
                return std::unexpected(FenParseError::InvalidPiecePlacementRankLength);
            // Only 8 ranks (7 separators) exist on a board.
            if(rankCount >= 7)
                return std::unexpected(FenParseError::InvalidPiecePlacementRankCount);

            // Move to the next rank towards white
            sq -= 16;
            rankSquareCount = 0;
            ++rankCount;
        }
        else
        {
            if(rankSquareCount >= 8)
                return std::unexpected(FenParseError::InvalidPiecePlacementRankLength);

            // Get the Piece enum from character in FEN
            std::size_t pieceIndex { pieceToChar.find(fenChar) };
            if(pieceIndex == std::string_view::npos || pieceIndex >= std::to_underlying(Piece::NUM_PIECES))
                return std::unexpected(FenParseError::InvalidPieceChar);

            // Update Piece Bitboards and mailbox
            position.pieceBitboards[pieceIndex] |= squareToBitboard(sq);
            position.pieceOnSquare[static_cast<std::size_t>(sq)] = static_cast<Piece>(pieceIndex);
            ++sq;
            ++rankSquareCount;
        }
    }
    // The final rank must be fully described, and exactly 8 ranks (7 separators) given.
    if(rankSquareCount != 8 || rankCount != 7)
        return std::unexpected(FenParseError::InvalidPiecePlacementRankCount);
    // Update Color Bitboards and Occupancy Bitboard
    position.pieceBitboards[std::to_underlying(Piece::WHITE_ALL)] = ( position.pieceBitboards[std::to_underlying(Piece::WHITE_PAWN)] | position.pieceBitboards[std::to_underlying(Piece::WHITE_KNIGHT)] |
                                        position.pieceBitboards[std::to_underlying(Piece::WHITE_BISHOP)] | position.pieceBitboards[std::to_underlying(Piece::WHITE_ROOK)] |
                                        position.pieceBitboards[std::to_underlying(Piece::WHITE_QUEEN)] | position.pieceBitboards[std::to_underlying(Piece::WHITE_KING)] );
    position.pieceBitboards[std::to_underlying(Piece::BLACK_ALL)] = ( position.pieceBitboards[std::to_underlying(Piece::BLACK_PAWN)] | position.pieceBitboards[std::to_underlying(Piece::BLACK_KNIGHT)] |
                                        position.pieceBitboards[std::to_underlying(Piece::BLACK_BISHOP)] | position.pieceBitboards[std::to_underlying(Piece::BLACK_ROOK)] |
                                        position.pieceBitboards[std::to_underlying(Piece::BLACK_QUEEN)] | position.pieceBitboards[std::to_underlying(Piece::BLACK_KING)] );
    position.pieceBitboards[std::to_underlying(Piece::ALL_PIECES)] = ( position.pieceBitboards[std::to_underlying(Piece::WHITE_ALL)] | position.pieceBitboards[std::to_underlying(Piece::BLACK_ALL)] );
    position.pieceBitboards[std::to_underlying(Piece::EMPTY)] = ~position.pieceBitboards[std::to_underlying(Piece::ALL_PIECES)];

    // 2. Active color. "w" means White moves next, "b" means Black moves next.
    fenStringStream >> fenChar;
    if(fenChar != 'w' && fenChar != 'b')
        return std::unexpected(FenParseError::InvalidActiveColorChar);
    position.sideToMove = (fenChar == 'w') ? Side::WHITE : Side::BLACK;
    fenStringStream >> fenChar;
    if(!std::isspace(static_cast<unsigned char>(fenChar)))
        return std::unexpected(FenParseError::MissingFieldSeparator);

    /*
     * 3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters:
     * "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside),
     * and/or "q" (Black can castle queenside). A move that temporarily prevents castling does not negate this notation.
     */
    while((fenStringStream >> fenChar) && !std::isspace(static_cast<unsigned char>(fenChar)))
    {
        if(validCastlingChars.find(fenChar) == std::string_view::npos)
            return std::unexpected(FenParseError::InvalidCastlingChar);

        switch(fenChar)
        {
            case 'K':
                position.castlingRights |= Castle::WHITE_KING_CASTLE;
                break;
            case 'Q':
                position.castlingRights |= Castle::WHITE_QUEEN_CASTLE;
                break;
            case 'k':
                position.castlingRights |= Castle::BLACK_KING_CASTLE;
                break;
            case 'q':
                position.castlingRights |= Castle::BLACK_QUEEN_CASTLE;
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
        position.enPassantSquare = LERFSquare::NO_SQ;
    }
    else
    {
        std::size_t fileIndex { fileToChar.find(fenChar) };
        if(fileIndex == std::string_view::npos || fileIndex >= std::to_underlying(File::NUM_FILES))
            return std::unexpected(FenParseError::InvalidEnPassantFile);

        fenStringStream >> fenChar;

        std::size_t rankIndex { rankToChar.find(fenChar) };
        if(rankIndex == std::string_view::npos || rankIndex >= std::to_underlying(Rank::NUM_RANKS))
            return std::unexpected(FenParseError::InvalidEnPassantRank);

        std::size_t epSq { rankIndex * 8 + fileIndex };
        if(epSq > std::to_underlying(LERFSquare::H8))
            return std::unexpected(FenParseError::InvalidEnPassantSquare);

        // An en passant target can only be on the rank directly behind
        // a double-pushed pawn of the side that just moved, i.e. rank 3
        // if White just pushed (Black to move) or rank 6 if Black just
        // pushed (White to move).
        int expectedRank { position.sideToMove == Side::WHITE ? std::to_underlying(Rank::RANK_6) : std::to_underlying(Rank::RANK_3) };
        if(static_cast<int>(rankIndex) != expectedRank)
            return std::unexpected(FenParseError::InvalidEnPassantSquare);

        position.enPassantSquare = static_cast<LERFSquare>(epSq);
    }
    fenStringStream >> fenChar;
    if(!std::isspace(static_cast<unsigned char>(fenChar)))
        return std::unexpected(FenParseError::MissingFieldSeparator);

    // 5. Halfmove clock: The number of halfmoves since the last capture or pawn advance, used for the fifty-move rule.
    fenStringStream >> std::skipws;
    fenStringStream >> fiftyMoves;
    if(fiftyMoves < 0 || fiftyMoves > 100)
        return std::unexpected(FenParseError::InvalidHalfmoveClock);
    position.fiftyMovesCount = fiftyMoves;

    // 6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.
    fenStringStream >> fullMoves;
    if(fullMoves < 1)
        return std::unexpected(FenParseError::InvalidFullmoveNumber);
    position.ply = (fullMoves - 1) * 2 + std::to_underlying(position.sideToMove);

    // 7. Compute position hash via Zobrist hashing.
    position.positionIdentity = position.calculatePositionHash();

    return position;
}

U64 Position::calculatePositionHash() const
{
    U64 hash { 0 };

    //Handle piece square keys including empty
    U64 sqBB {};
    for(int sq { std::to_underlying(LERFSquare::A1) }; sq < std::to_underlying(LERFSquare::NUM_SQUARES); ++sq)
    {
        sqBB = squareToBitboard(sq);
        // Loop through piece types
        for(int pieceType { std::to_underlying(Piece::EMPTY) }; pieceType < std::to_underlying(Piece::NUM_PIECES); ++pieceType)
        {
            if(this->pieceBitboards[pieceType] & sqBB)
            {
                hash ^= this->pieceSquareKeys[sq][pieceType];
                break;
            }
        }
    }

    //Handle side to move
    if(this->sideToMove == Side::BLACK)
    {
        hash ^= this->sideToMoveKey;
    }

    //Handle castling rights
    hash ^= this->castlingRightKeys[std::to_underlying(this->castlingRights)];

    //Handle enPassant File
    if(this->enPassantSquare != LERFSquare::NO_SQ)
    {
        int file { fileOf(std::to_underlying(this->enPassantSquare)) };
        hash ^= this->enPassantFileKeys[file];
    }

    return hash;
}

U64 Position::getPieceBitboard(Piece piece) const
{
    return this->pieceBitboards[std::to_underlying(piece)];
}

Side Position::getSideToMove() const
{
    return this->sideToMove;
}

Castle Position::getCastlingRights() const
{
    return this->castlingRights;
}

LERFSquare Position::getEnPassantSquare() const
{
    return this->enPassantSquare;
}

int Position::getFiftyMovesCount() const
{
    return this->fiftyMovesCount;
}

int Position::getPly() const
{
    return this->ply;
}

U64 Position::getPositionIdentity() const
{
    return this->positionIdentity;
}

void Position::print() const
{
    // 1. Print 8x8 board to console
    U64 sqBB {};
    char pieceChar {};
    for(int rank { std::to_underlying(Rank::RANK_8) }; rank >= std::to_underlying(Rank::RANK_1); --rank)
    {
        for(int file { std::to_underlying(File::FILE_A) }; file <= std::to_underlying(File::FILE_H); ++file)
        {
            sqBB = squareToBitboard(rank * 8 + file);

            for(std::size_t pieceType { std::to_underlying(Piece::EMPTY) }; pieceType < std::to_underlying(Piece::NUM_PIECES); ++pieceType)
            {
                if(this->pieceBitboards[pieceType] & sqBB)
                {
                    pieceChar = pieceToChar[pieceType];
                    std::print("{} ", pieceChar);
                    break;
                }
            }
        }
        std::println();
    }

    // 2. Print other state data to console
    std::println("\nEnPassant Square: {}", std::to_underlying(this->enPassantSquare));
    std::println("Castling Rights: {}", std::to_underlying(this->castlingRights));
    std::println("Fifty Moves Count: {}", this->fiftyMovesCount);
    std::println("Ply: {}", this->ply);
    std::println("Position ID: {}", this->positionIdentity);
    std::println("Side to Move: {}", std::to_underlying(this->sideToMove));
}

Piece Position::pieceOn(LERFSquare square) const
{
    return this->pieceOnSquare[std::to_underlying(square)];
}

/*
 * Alongside the bitboard/mailbox bookkeeping, incrementally maintains
 * positionIdentity: square must be empty on entry (every call site
 * removes whatever was on the destination first), so this XORs out
 * the square's "empty" key and XORs in its new piece key -- see
 * calculatePositionHash(), which folds an empty square's key into the
 * hash exactly like an occupied one.
 */
void Position::addPiece(Piece piece, LERFSquare square)
{
    U64 bit { squareToBitboard(std::to_underlying(square)) };
    Piece allSide { allPiecesOf(pieceSide(piece)) };
    this->pieceBitboards[std::to_underlying(piece)] |= bit;
    this->pieceBitboards[std::to_underlying(allSide)] |= bit;
    this->pieceBitboards[std::to_underlying(Piece::ALL_PIECES)] |= bit;
    this->pieceBitboards[std::to_underlying(Piece::EMPTY)] &= ~bit;
    this->pieceOnSquare[std::to_underlying(square)] = piece;

    std::size_t sq { static_cast<std::size_t>(std::to_underlying(square)) };
    this->positionIdentity ^= this->pieceSquareKeys[sq][std::to_underlying(Piece::EMPTY)];
    this->positionIdentity ^= this->pieceSquareKeys[sq][std::to_underlying(piece)];
}

/*
 * See addPiece()'s comment: the mirror-image incremental hash update,
 * XORing out square's piece key and XORing in its new "empty" key.
 */
void Position::removePiece(Piece piece, LERFSquare square)
{
    U64 bit { squareToBitboard(std::to_underlying(square)) };
    Piece allSide { allPiecesOf(pieceSide(piece)) };
    this->pieceBitboards[std::to_underlying(piece)] &= ~bit;
    this->pieceBitboards[std::to_underlying(allSide)] &= ~bit;
    this->pieceBitboards[std::to_underlying(Piece::ALL_PIECES)] &= ~bit;
    this->pieceBitboards[std::to_underlying(Piece::EMPTY)] |= bit;
    this->pieceOnSquare[std::to_underlying(square)] = Piece::EMPTY;

    std::size_t sq { static_cast<std::size_t>(std::to_underlying(square)) };
    this->positionIdentity ^= this->pieceSquareKeys[sq][std::to_underlying(piece)];
    this->positionIdentity ^= this->pieceSquareKeys[sq][std::to_underlying(Piece::EMPTY)];
}

UnmakeState Position::makeMove(Move move)
{
    UnmakeState saved {
        .capturedPiece = Piece::EMPTY,
        .previousCastlingRights = this->castlingRights,
        .previousEnPassantSquare = this->enPassantSquare,
        .previousFiftyMovesCount = this->fiftyMovesCount,
        .previousPositionIdentity = this->positionIdentity
    };

    LERFSquare from { move.from() };
    LERFSquare to { move.to() };
    Side movingSide { this->sideToMove };
    Piece movingPiece { this->pieceOn(from) };

    // Remove whatever is captured, before the mover's own piece lands on `to`.
    if(move.isEnPassant())
    {
        LERFSquare capturedSquare { enPassantCapturedPawnSquare(movingSide, to) };
        saved.capturedPiece = this->pieceOn(capturedSquare);
        this->removePiece(saved.capturedPiece, capturedSquare);
    }
    else if(move.isCapture())
    {
        saved.capturedPiece = this->pieceOn(to);
        this->removePiece(saved.capturedPiece, to);
    }

    this->removePiece(movingPiece, from);
    Piece placedPiece { move.isPromotion() ? move.promotionPieceType(movingSide) : movingPiece };
    this->addPiece(placedPiece, to);

    if(move.isCastle())
    {
        Piece rookPiece { sidedPiece(movingSide, Piece::WHITE_ROOK) };
        LERFSquare rookFrom { castleRookFrom(movingSide, move.flag()) };
        LERFSquare rookTo { castleRookTo(movingSide, move.flag()) };
        this->removePiece(rookPiece, rookFrom);
        this->addPiece(rookPiece, rookTo);
    }

    // Castling-rights and en passant-square changes each touch at
    // most one Zobrist key (indexed by the whole Castle bitmask, or
    // by file), so -- unlike piece placement above -- they're XORed
    // in/out directly here rather than through a shared helper.
    Castle newCastlingRights { clearCastleRights(this->castlingRights, castleRightsLostAt(from)) };
    newCastlingRights = clearCastleRights(newCastlingRights, castleRightsLostAt(to));
    if(newCastlingRights != this->castlingRights)
    {
        this->positionIdentity ^= this->castlingRightKeys[std::to_underlying(this->castlingRights)];
        this->positionIdentity ^= this->castlingRightKeys[std::to_underlying(newCastlingRights)];
        this->castlingRights = newCastlingRights;
    }

    LERFSquare newEnPassantSquare { move.isDoublePawnPush()
        ? static_cast<LERFSquare>((std::to_underlying(from) + std::to_underlying(to)) / 2)
        : LERFSquare::NO_SQ };
    if(this->enPassantSquare != LERFSquare::NO_SQ)
        this->positionIdentity ^= this->enPassantFileKeys[fileOf(std::to_underlying(this->enPassantSquare))];
    if(newEnPassantSquare != LERFSquare::NO_SQ)
        this->positionIdentity ^= this->enPassantFileKeys[fileOf(std::to_underlying(newEnPassantSquare))];
    this->enPassantSquare = newEnPassantSquare;

    bool isPawnMove { movingPiece == Piece::WHITE_PAWN || movingPiece == Piece::BLACK_PAWN };
    this->fiftyMovesCount = (isPawnMove || move.isCapture()) ? 0 : this->fiftyMovesCount + 1;

    ++this->ply;
    this->sideToMove = (movingSide == Side::WHITE) ? Side::BLACK : Side::WHITE;
    this->positionIdentity ^= this->sideToMoveKey;

    return saved;
}

void Position::unmakeMove(Move move, const UnmakeState& saved)
{
    this->sideToMove = (this->sideToMove == Side::WHITE) ? Side::BLACK : Side::WHITE;
    --this->ply;

    LERFSquare from { move.from() };
    LERFSquare to { move.to() };
    Side movingSide { this->sideToMove };

    Piece placedPiece { this->pieceOn(to) };
    Piece originalPiece { move.isPromotion() ? sidedPiece(movingSide, Piece::WHITE_PAWN) : placedPiece };

    this->removePiece(placedPiece, to);
    this->addPiece(originalPiece, from);

    if(move.isEnPassant())
    {
        LERFSquare capturedSquare { enPassantCapturedPawnSquare(movingSide, to) };
        this->addPiece(saved.capturedPiece, capturedSquare);
    }
    else if(move.isCapture())
    {
        this->addPiece(saved.capturedPiece, to);
    }

    if(move.isCastle())
    {
        Piece rookPiece { sidedPiece(movingSide, Piece::WHITE_ROOK) };
        LERFSquare rookFrom { castleRookFrom(movingSide, move.flag()) };
        LERFSquare rookTo { castleRookTo(movingSide, move.flag()) };
        this->removePiece(rookPiece, rookTo);
        this->addPiece(rookPiece, rookFrom);
    }

    this->castlingRights = saved.previousCastlingRights;
    this->enPassantSquare = saved.previousEnPassantSquare;
    this->fiftyMovesCount = saved.previousFiftyMovesCount;
    this->positionIdentity = saved.previousPositionIdentity;
}
