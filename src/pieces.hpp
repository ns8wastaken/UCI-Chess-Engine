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


    struct HistoryMove
    {
        int capturedPiece = Piece::NONE;
        uint8_t fromSquare = 64;
        uint8_t toSquare = 64;
        uint8_t enPassantSquare = 64;
    };


    constexpr int getPieceValue(int pieceType)
    {
        switch (pieceType) {
            case PAWN:   return 100;
            case KNIGHT: return 300;
            case BISHOP: return 300;
            case ROOK:   return 500;
            case QUEEN:  return 900;
            case KING:   return 31415926;
        }

        return 0;
    }


    constexpr char getPieceChar(int piece)
    {
        switch (piece) {
            // White
            case W_PAWN:   return 'P';
            case W_KNIGHT: return 'N';
            case W_BISHOP: return 'B';
            case W_ROOK:   return 'R';
            case W_QUEEN:  return 'Q';
            case W_KING:   return 'K';

            // Black
            case B_PAWN:   return 'p';
            case B_KNIGHT: return 'n';
            case B_BISHOP: return 'b';
            case B_ROOK:   return 'r';
            case B_QUEEN:  return 'q';
            case B_KING:   return 'k';
        }

        return '\0';
    };


    constexpr char getPromotionChar(uint8_t promotion)
    {
        switch (promotion) {
            case Promotion::P_NONE:   return '\0';
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
            case 'P': return W_PAWN;
            case 'N': return W_KNIGHT;
            case 'B': return W_BISHOP;
            case 'R': return W_ROOK;
            case 'Q': return W_QUEEN;
            case 'K': return W_KING;

            // Black
            case 'p': return B_PAWN;
            case 'n': return B_KNIGHT;
            case 'b': return B_BISHOP;
            case 'r': return B_ROOK;
            case 'q': return B_QUEEN;
            case 'k': return B_KING;
        }

        return Piece::NONE;
    };

}
