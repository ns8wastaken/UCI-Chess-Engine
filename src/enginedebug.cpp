#include "engine.hpp"


uint64_t Engine::perft(const int depth)
{
    if (depth == 0)
        return 1ULL;

    MoveList moveList = getPseudoLegalMoves();

    uint64_t totalNodes = 0;

    for (int i = 0; i < moveList.used; ++i) {
        const Pieces::Move& move = moveList.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        totalNodes += perft(depth - 1);

        undoMove();
    }

    return totalNodes;
}


uint64_t Engine::divide(const int depth)
{
    MoveList moveList = getPseudoLegalMoves();

    uint64_t totalNodes = 0;

    for (int i = 0; i < moveList.used; ++i) {
        const Pieces::Move& move = moveList.moves[i];

        if (!isLegalCastle(move)) continue;

        makeMove(move);

        if (wasIllegalMove()) {
            undoMove();
            continue;
        }

        uint64_t moveNodes = perft(depth - 1);
        std::cout << Utils::toUCI(move) << ": " << moveNodes << "\n";
        totalNodes += moveNodes;

        undoMove();
    }

    return totalNodes;
}
