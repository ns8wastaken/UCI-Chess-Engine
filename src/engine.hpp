#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <bit>
#include <fstream>

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
    int plyCount = 0;

    void setColor(bool color);
    void flipColor();
    Board board;

    bool isWhiteTurn = true;

    Pieces::Move bestMove = {};

    void loadFEN(const std::vector<std::string>& FEN);
    std::string getFEN() const;


    // Engine functions
    int evaluateBoard() const;
    int quiescentSearch(int alpha, const int beta);

    Bitboard generatePieceMoves(const Square& square, const int& piece) const;
    MoveList getPseudoLegalMoves() const;

    void makeMove(const Pieces::Move& move);
    void makeUCIMove(const std::string& UCI_Move);

    void undoMove();

    bool isAttacked(const Square square);
    bool isLegalCastle(const Pieces::Move& move);
    bool wasIllegalMove();


    // Movegen
    void randomMove();
    int16_t negaMax(const int& depth);
    int16_t alphaBeta(const int& depth, int16_t alpha, const int16_t& beta);

    std::string getEngineMove();

    uint64_t perft(const int depth);
    uint64_t divide(const int depth);

    static constexpr int16_t INF_VALUE = 30000;
};
