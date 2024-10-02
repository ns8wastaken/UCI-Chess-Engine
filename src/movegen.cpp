#include "engine.hpp"


void Engine::randomMove()
{
    MoveList moves = generateAllMoves();

    MoveList legalMoves = {};

    for (int i = 0; i < moves.used; ++i) {
        const Pieces::Move& move = moves.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        legalMoves.moves[legalMoves.used++] = move;

        undoMove();
    }

    bestMove = legalMoves.moves[Utils::randomInt(0, legalMoves.used - 1)];
}


int Engine::negaMax(int depth)
{
    if (depth == 0) return evaluateBoard();

    int max = -std::numeric_limits<int>::max();

    MoveList moves = generateAllMoves();

    for (int i = 0; i < moves.used; ++i) {
        const Pieces::Move& move = moves.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        int score = -negaMax(depth - 1);

        undoMove();

        if (score > max) {
            if (depth == Settings::maxPlyDepth)
                bestMove = move;

            max = score;
        }
    }

    return max;
}


int Engine::alphaBeta(int depth, int alpha, int beta)
{
    if (depth == 0)
        // return quiesce(alpha, beta);
        return evaluateBoard();

    int bestValue = -std::numeric_limits<int>::max();

    MoveList moves = generateAllMoves();

    for (int i = 0; i < moves.used; ++i) {
        const Pieces::Move& move = moves.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        int score = -alphaBeta(depth - 1, -beta, -alpha);

        undoMove();

        if (score > bestValue) {
            bestValue = score;

            if (depth == Settings::maxPlyDepth)
                bestMove = move;

            alpha = std::max(alpha, score);
        }

        if (score >= beta)
            return bestValue;
    }

    return bestValue;
}
