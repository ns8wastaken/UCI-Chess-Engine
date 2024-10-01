#pragma once
#include <array>

#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<Bitboard, Pieces::Piece::PIECE_COUNT> BitboardArray;

typedef std::array<int, 64ULL> Mailbox;


struct Board
{
    struct PrecomputedMoves
    {
        std::array<Bitboard, 64ULL> knightMoves = {};
        std::array<Bitboard, 64ULL> kingMoves   = {};
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
        std::array<HistoryState, 1000ULL> history{};
        int used = 0ULL;
    } history;


    int plyCount = 0;

    char castlingFlags     = 0;
    Square enPassantSquare = 64;

    BitboardArray bitboards = {};
    Mailbox mailbox         = {};

    Bitboard attackedSquares = 0ULL;

    // 0: Black
    // 1: White
    Bitboard occupiedSquares[2] = {0ULL, 0ULL};

    void precomputeMoves();
};
