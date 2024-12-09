#pragma once
#include <array>

#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<Bitboard, static_cast<std::size_t>(Pieces::Piece::PIECE_COUNT)> BitboardArray;

typedef std::array<Pieces::Piece, 64ULL> Mailbox; // Array of pieces (not piece types)


struct Board
{
    struct PrecomputedMoves
    {
        Bitboard knightMoves[64] = {};
        Bitboard kingMoves[64]   = {};
    } precomputedMoves;


    struct HistoryState
    {
        BitboardArray bitboards = {};
        Mailbox mailbox         = {};

        Square enPassantSquare = 0;
        char castlingFlags     = 0;

        Bitboard occupiedSquares[2] = {0ULL, 0ULL};

        HistoryState() = default;
        HistoryState(const Board& board)
            : bitboards(board.bitboards),
              mailbox(board.mailbox),

              enPassantSquare(board.enPassantSquare),
              castlingFlags(board.castlingFlags),

              occupiedSquares{board.occupiedSquares[0], board.occupiedSquares[1]}
        {}
    };


    struct HistoryList
    {
        HistoryState history[500] = {};

        int used = 0;
    } history;


    char castlingFlags     = 0;
    Square enPassantSquare = 64;

    BitboardArray bitboards = {};
    Mailbox mailbox         = {};

    Bitboard attackedSquares = 0ULL;

    // 0: Black
    // 1: White
    Bitboard occupiedSquares[2] = {0ULL, 0ULL};

    void precomputeMoves();

    void placePiece(const Pieces::Piece& piece, const bool& isPieceWhite, const Square& square);
    void removePiece(const Pieces::Piece& piece, const bool& isPieceWhite, const Square& square);
};
