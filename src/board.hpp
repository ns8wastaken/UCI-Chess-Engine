#pragma once
#include <array>

#include "utils.hpp"
#include "pieces.hpp"


struct Board
{
    struct PrecomputedMoves
    {
        std::array<Bitboard, 64> knightMoves;
        std::array<Bitboard, 64> kingMoves;
    };

    PrecomputedMoves precomputedMoves;

    std::array<uint64_t, Pieces::Piece::PIECE_COUNT> bitboards{};
    std::array<int, 64ULL> mailbox{};

    // Occupied white squares
    Bitboard w_occupiedSquares = 0ULL;
    // Occupied black squares
    Bitboard b_occupiedSquares = 0ULL;

    void precomputeMoves();
};
