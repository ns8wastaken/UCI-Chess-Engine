#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <bit>

#include "settings.hpp"
#include "board.hpp"
#include "utils.hpp"
#include "pieces.hpp"


struct Engine
{
    struct MoveList
    {
        Pieces::Move moves[256] = {};

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
    } ownPiece, enemyPiece;


    Engine();


    // Board stuff
    void setColor(bool color);
    void flipColor();
    Board board;

    bool isWhiteTurn = true;

    Pieces::Move bestMove = {};

    void loadFEN(const std::vector<std::string>& FEN);

    // Engine functions
    int evaluateBoard() const;
    int quiescentSearch(int alpha, const int beta);

    Bitboard generatePieceMoves(const Square& square, const int& piece) const;
    MoveList generateAllMoves() const;

    void makeMove(const Pieces::Move& move);
    void makeUCIMove(const std::string& UCI_Move);

    void undoMove();

    bool isAttacked(const Square square);
    bool isLegalCastle(const Pieces::Move& move);
    bool wasIllegalMove();

    // Movegen
    void randomMove();
    int negaMax(int depth);
    int alphaBeta(const int depth, int alpha, const int beta);

    std::string getEngineMove();

    uint64_t perft(const int depth);
    uint64_t divide(const int depth);
};
