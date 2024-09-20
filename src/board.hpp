#pragma once
#include <array>
#include <stack>

#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<uint64_t, Pieces::Piece::PIECE_COUNT> BitboardArray;


struct Board
{
    struct CastlingRights
    {
        bool whiteKingside = true;
        bool whiteQueenside = true;
        bool blackKingside = true;
        bool blackQueenside = true;
    };

    struct HistoryState
    {
        BitboardArray bitboards;
        Square enPassantSquare;
        CastlingRights castlingRights;
        std::array<int, 64ULL> mailbox;
    };

    struct PrecomputedMoves
    {
        std::array<Bitboard, 64> knightMoves{};
        std::array<Bitboard, 64> kingMoves{};
    };

    Square enPassantSquare = 0;

    PrecomputedMoves precomputedMoves;
    CastlingRights castlingRights;

    std::stack<HistoryState> history{};

    BitboardArray bitboards{};
    std::array<int, 64ULL> mailbox{};

    Bitboard attackedSquares = 0ULL;

    // Occupied white squares
    Bitboard w_occupiedSquares = 0ULL;
    // Occupied black squares
    Bitboard b_occupiedSquares = 0ULL;

    void precomputeMoves();
};
