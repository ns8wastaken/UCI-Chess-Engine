#pragma once
#include <cstdint>


namespace Pieces
{

    enum PieceType
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,

        PIECE_TYPE_COUNT
    };


    enum Piece
    {
        W_PAWN = 0,
        B_PAWN,

        W_KNIGHT,
        B_KNIGHT,

        W_BISHOP,
        B_BISHOP,

        W_ROOK,
        B_ROOK,

        W_QUEEN,
        B_QUEEN,

        W_KING,
        B_KING,

        PIECE_COUNT,
        NONE
    };


    constexpr int pieceValues[6] = {
        100, 300, 300, 500, 900, 31415926
    };


    enum class MoveFlag
    {
        CASTLING         = 0,
        PROMOTION_KNIGHT = 2,
        PROMOTION_BISHOP = 4,
        PROMOTION_ROOK   = 6,
        PROMOTION_QUEEN  = 8
    };


    struct Move
    {
        uint8_t fromSquare      = 64;
        uint8_t toSquare        = 64;
        int promotionPieceType  = PieceType::PIECE_TYPE_COUNT;
        bool isCastle           = false;
    };


    constexpr char getPieceTypeChar(int piece)
    {
        switch (piece) {
            case PieceType::PAWN:   return 'p';
            case PieceType::KNIGHT: return 'n';
            case PieceType::BISHOP: return 'b';
            case PieceType::ROOK:   return 'r';
            case PieceType::QUEEN:  return 'q';
            case PieceType::KING:   return 'k';
        }

        return '\0';
    };


    constexpr char getPieceChar(int piece)
    {
        switch (piece) {
            // White
            case Piece::W_PAWN:   return 'P';
            case Piece::W_KNIGHT: return 'N';
            case Piece::W_BISHOP: return 'B';
            case Piece::W_ROOK:   return 'R';
            case Piece::W_QUEEN:  return 'Q';
            case Piece::W_KING:   return 'K';

            // Black
            case Piece::B_PAWN:   return 'p';
            case Piece::B_KNIGHT: return 'n';
            case Piece::B_BISHOP: return 'b';
            case Piece::B_ROOK:   return 'r';
            case Piece::B_QUEEN:  return 'q';
            case Piece::B_KING:   return 'k';
        }

        return '\0';
    };


    constexpr int getPieceTypeFromChar(char c)
    {
        switch (c) {
            case 'p':
            case 'P': return PieceType::PAWN;

            case 'n':
            case 'N': return PieceType::KNIGHT;

            case 'b':
            case 'B': return PieceType::BISHOP;

            case 'r':
            case 'R': return PieceType::ROOK;

            case 'q':
            case 'Q': return PieceType::QUEEN;

            case 'k':
            case 'K': return PieceType::KING;
        }

        return PieceType::PIECE_TYPE_COUNT;
    };


    constexpr int getPieceFromChar(char c)
    {
        switch (c) {
            // White
            case 'P': return Piece::W_PAWN;
            case 'N': return Piece::W_KNIGHT;
            case 'B': return Piece::W_BISHOP;
            case 'R': return Piece::W_ROOK;
            case 'Q': return Piece::W_QUEEN;
            case 'K': return Piece::W_KING;

            // Black
            case 'p': return Piece::B_PAWN;
            case 'n': return Piece::B_KNIGHT;
            case 'b': return Piece::B_BISHOP;
            case 'r': return Piece::B_ROOK;
            case 'q': return Piece::B_QUEEN;
            case 'k': return Piece::B_KING;
        }

        return Piece::NONE;
    };

}
