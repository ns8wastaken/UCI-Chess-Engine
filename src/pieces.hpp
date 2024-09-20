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
        KING
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


    enum Promotion
    {
        P_NONE,

        P_W_KNIGHT,
        P_B_KNIGHT,

        P_W_BISHOP,
        P_B_BISHOP,

        P_W_ROOK,
        P_B_ROOK,

        P_W_QUEEN,
        P_B_QUEEN
    };


    struct Move
    {
        uint8_t fromSquare = 64;
        uint8_t toSquare = 64;
        uint8_t promotion = 0;
    };


    constexpr int getPieceValue(int pieceType)
    {
        switch (pieceType) {
            case PieceType::PAWN:   return 100;
            case PieceType::KNIGHT: return 300;
            case PieceType::BISHOP: return 300;
            case PieceType::ROOK:   return 500;
            case PieceType::QUEEN:  return 900;
            case PieceType::KING:   return 31415926;
        }

        return 0;
    }


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


    constexpr char getPromotionChar(uint8_t promotion)
    {
        switch (promotion) {
            case Promotion::P_NONE:     return '\0';

            case Promotion::P_W_KNIGHT: return 'N';
            case Promotion::P_B_KNIGHT: return 'n';

            case Promotion::P_W_BISHOP: return 'B';
            case Promotion::P_B_BISHOP: return 'b';

            case Promotion::P_W_ROOK:   return 'R';
            case Promotion::P_B_ROOK:   return 'r';

            case Promotion::P_W_QUEEN:  return 'Q';
            case Promotion::P_B_QUEEN:  return 'q';
        }

        return '\0';
    }


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


    constexpr int getPromotionFromChar(char c)
    {
        switch (c) {
            case 'N': return Promotion::P_W_KNIGHT;
            case 'B': return Promotion::P_W_BISHOP;
            case 'R': return Promotion::P_W_ROOK;
            case 'Q': return Promotion::P_W_QUEEN;

            case 'n': return Promotion::P_B_KNIGHT;
            case 'b': return Promotion::P_B_BISHOP;
            case 'r': return Promotion::P_B_ROOK;
            case 'q': return Promotion::P_B_QUEEN;
        }

        return Promotion::P_NONE;
    }

}
