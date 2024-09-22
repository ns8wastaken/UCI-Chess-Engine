#pragma once
#include <array>
#include <stack>

#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<uint64_t, Pieces::Piece::PIECE_COUNT> BitboardArray;

typedef std::array<int, 64ULL> Mailbox;


struct Board
{
    struct CastlingRights
    {
        bool whiteKingside  = true;
        bool whiteQueenside = true;
        bool blackKingside  = true;
        bool blackQueenside = true;
    } castlingRights;

    struct PrecomputedMoves
    {
        std::array<Bitboard, 64> knightMoves{};
        std::array<Bitboard, 64> kingMoves{};
    } precomputedMoves;

    struct HistoryState
    {
        BitboardArray bitboards;
        Mailbox mailbox;

        Square enPassantSquare;
        CastlingRights castlingRights;

        Bitboard w_occupiedSquares = 0ULL;
        Bitboard b_occupiedSquares = 0ULL;

        int plyCount   = 0;
        int moveNumber = 0;

        HistoryState(const Board& board)
            : bitboards(board.bitboards),
              mailbox(board.mailbox),

              enPassantSquare(board.enPassantSquare),
              castlingRights(board.castlingRights),

              w_occupiedSquares(board.w_occupiedSquares),
              b_occupiedSquares(board.b_occupiedSquares),

              plyCount(board.plyCount),
              moveNumber(board.moveNumber)
        {}
    };

    int plyCount   = 0;
    int moveNumber = 0;

    Square enPassantSquare = 0;

    std::stack<HistoryState> history{};

    BitboardArray bitboards{};
    Mailbox mailbox{};

    Bitboard attackedSquares = 0ULL;

    // Occupied white squares
    Bitboard w_occupiedSquares = 0ULL;
    // Occupied black squares
    Bitboard b_occupiedSquares = 0ULL;

    void precomputeMoves();
};
