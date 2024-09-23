#include "engine.hpp"


uint64_t Engine::perft(const int depth)
{
    if (depth == 0)
        return 1ULL;

    uint64_t nodes = 0;

    MoveList move_list = generateAllMoves();

    for (int i = 0; i < move_list.used; ++i) {
        Pieces::Move move = move_list.moves[i];

        if (!isLegalMove(move)) continue;

        makeMove(move);

        nodes += perft(depth - 1);

        undoMove();
    }

    return nodes;
}


uint64_t Engine::divide(const int depth)
{
    const auto start = std::chrono::high_resolution_clock::now();

    MoveList move_list = generateAllMoves();

    uint64_t totalNodes = 0;

    for (int i = 0; i < move_list.used; ++i) {
        Pieces::Move move = move_list.moves[i];

        if (!isLegalMove(move)) continue;

        makeMove(move);

        uint64_t moveNodes = perft(depth - 1);
        std::cout << Utils::toUCI(move) << ": " << moveNodes << "\n";
        totalNodes += moveNodes;

        undoMove();
    }

    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << "\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

    return totalNodes;
}
