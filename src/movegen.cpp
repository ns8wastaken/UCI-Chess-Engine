#include "engine.hpp"


int Engine::quiescentSearch(int alpha, const int beta)
{
    int stand_pat = evaluateBoard();

    if (stand_pat >= beta)
        return beta;

    if (alpha < stand_pat)
        alpha = stand_pat;

    // alpha = std::max(alpha, stand_pat);

    MoveList moves = generateAllMoves();

    for (int i = 0; i < moves.used; ++i) {
        Pieces::Move move = moves.moves[i];

        if (board.mailbox[move.toSquare] == Pieces::Piece::NONE) continue;

        makeMove(move);

        int score = -quiescentSearch(-beta, -alpha);

        undoMove();

        if (score >= beta)
            return beta;

        // alpha = std::max(alpha, score);

        if (score > alpha)
            alpha = score;
    }

    return alpha;
}



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


int Engine::alphaBeta(const int depth, int alpha, const int beta)
{
    if (depth == 0)
        // return quiescentSearch(alpha, beta);
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
