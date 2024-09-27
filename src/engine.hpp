#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <bit>

#include "board.hpp"
#include "utils.hpp"
#include "pieces.hpp"


typedef std::array<Pieces::Move, 256ULL> MoveArray;


struct MoveList
{
    MoveArray moves{};
    int used = 0ULL;
};


struct Piece
{
    int PAWN;
    int KNIGHT;
    int BISHOP;
    int ROOK;
    int QUEEN;
    int KING;
};


struct Engine
{
    Engine();
    void setColor(bool color);
    void flipColor();
    Board board;

    Piece ownPiece = {
        Pieces::Piece::W_PAWN,
        Pieces::Piece::W_KNIGHT,
        Pieces::Piece::W_BISHOP,
        Pieces::Piece::W_ROOK,
        Pieces::Piece::W_QUEEN,
        Pieces::Piece::W_KING
    };

    Piece enemyPiece = {
        Pieces::Piece::B_PAWN,
        Pieces::Piece::B_KNIGHT,
        Pieces::Piece::B_BISHOP,
        Pieces::Piece::B_ROOK,
        Pieces::Piece::B_QUEEN,
        Pieces::Piece::B_KING
    };

    bool isWhiteTurn = true;

    void loadFEN(const std::vector<std::string>& FEN);

    int evaluateBoard();

    Bitboard generatePieceMoves(const Square square, const int piece);
    MoveList generateAllMoves();

    void makeMove(const Pieces::Move& move);
    void makeUCIMove(const std::string& UCI_Move);

    void undoMove();

    bool isAttacked(const Square square);
    bool isLegalCastle(const Pieces::Move& move);
    bool wasLegalMove();

    std::string getEngineMove();

    uint64_t perft(const int depth);
    uint64_t divide(const int depth);
};
