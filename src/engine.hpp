#pragma once
#include <string>
#include <vector>

#include "board.hpp"
#include "utils.hpp"
#include "pieces.hpp"


struct Engine
{
    Engine();

    bool isWhiteTurn = true;

    void loadFEN(const std::string& FEN);

    Bitboard generatePieceMoves(int square);
    std::array<Pieces::Move, 218ULL> generateAllMoves();

    void makeMove(Pieces::Move move);
    void makeUCIMove(const std::string& UCI_Move);

    std::string getEngineMove();

    Board board;
};
