#pragma once
#include <array>
#include <stack>

#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<Bitboard, Pieces::Piece::PIECE_COUNT> BitboardArray;

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


        Bitboard occupiedSquares[2] = {0ULL, 0ULL};

        HistoryState(const Board& board)
            : bitboards(board.bitboards),
              mailbox(board.mailbox),

              enPassantSquare(board.enPassantSquare),
              castlingRights(board.castlingRights),

              occupiedSquares{board.occupiedSquares[0], board.occupiedSquares[1]}
        {}
    };


    int plyCount = 0;

    Square enPassantSquare = 64;

    std::stack<HistoryState> history{};

    BitboardArray bitboards{};
    Mailbox mailbox{};

    Bitboard attackedSquares = 0ULL;

    // 0: Black
    // 1: White
    Bitboard occupiedSquares[2] = {0ULL, 0ULL};

    void precomputeMoves();
};
