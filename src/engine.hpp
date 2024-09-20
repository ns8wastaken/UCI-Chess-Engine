#pragma once
#include <string>
#include <vector>

#include "board.hpp"
#include "utils.hpp"
#include "pieces.hpp"


struct HistoryState
{
    Board board;
    Square enPassantSquare;
    Board::CastlingRights castlingRights;
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
    Square enPassantSquare = 0;

    void loadFEN(const std::vector<std::string>& FEN);

    Bitboard generatePieceMoves(int square, int piece);
    std::array<Pieces::Move, 218ULL> generateAllMoves();

    bool isAttacked(Square square);

    void makeMove(Pieces::Move move);
    void makeUCIMove(const std::string& UCI_Move);

    void undoMove();

    std::string getEngineMove();
};
