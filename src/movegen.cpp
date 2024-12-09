#include "engine.hpp"


int Engine::quiescentSearch(int alpha, const int beta)
{
    int stand_pat = evaluateBoard();

    if (stand_pat >= beta)
        return beta;

    if (alpha < stand_pat)
        alpha = stand_pat;

    // alpha = std::max(alpha, stand_pat);

    MoveList moves = getPseudoLegalMoves();

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
    MoveList moves = getPseudoLegalMoves();

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


int16_t Engine::negaMax(const int& depth)
{
    if (depth == 0) return evaluateBoard();

    int16_t max = -INF_VALUE;

    MoveList moveList = getPseudoLegalMoves();

    int legalMovesMade = 0;

    for (int i = 0; i < moveList.used; ++i) {
        const Pieces::Move& move = moveList.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        ++legalMovesMade;
        int16_t score = -negaMax(depth - 1);

        undoMove();

        if (score > max) {
            if (depth == Settings::searchDepth)
                bestMove = move;

            max = score;
        }
    }

    // No legal moves
    if (legalMovesMade == 0) {
        // Checkmate
        if (isAttacked(static_cast<Square>(std::countr_zero(board.bitboards[ownPiece.KING]))))
            return -INF_VALUE + plyCount;

        // Stalemate
        return 0;
    }

    return max;
}


int16_t Engine::alphaBeta(const int& depth, int16_t alpha, const int16_t& beta)
{
    if (depth == 0)
        // return quiescentSearch(alpha, beta);
        return evaluateBoard();

    int16_t bestValue = -INF_VALUE;

    MoveList moves = getPseudoLegalMoves();

    for (int i = 0; i < moves.used; ++i) {
        const Pieces::Move& move = moves.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        int16_t score = -alphaBeta(depth - 1, -beta, -alpha);

        undoMove();

        if (score > bestValue) {
            bestValue = score;

            if (depth == Settings::searchDepth)
                bestMove = move;

            alpha = std::max(alpha, score);
        }

        if (score >= beta)
            return bestValue;
    }

    return bestValue;
}
