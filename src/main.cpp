#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#include "utils.hpp"
#include "engine.cpp"


#define ifcommand(x) if (command == x)
#define elifcommand(x) else if (command == x)
#define ifsplitcommand(i, x) if (splitCommand[i] == x)
#define elifsplitcommand(i, x) else if (splitCommand[i] == x)


constexpr const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


void printBitboard(uint64_t bitboard)
{
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;

            if ((bitboard >> square) & 1ULL) {
                std::cout << "1 ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


std::vector<std::string> splitStr(const std::string& str)
{
    std::vector<std::string> list;
    std::size_t currIndex = 0;
    std::size_t nextIndex;

    while (currIndex < str.length()) {
        nextIndex = str.find_first_of(' ', currIndex);

        if (nextIndex == std::string::npos) {
            list.push_back(str.substr(currIndex));
            break;
        }

        list.push_back(str.substr(currIndex, nextIndex - currIndex));
        currIndex = nextIndex + 1;
    }

    return list;
}


int main()
{
    Engine engine;

    std::string command = "";
    std::vector<std::string> splitCommand = splitStr(command);

    while (std::getline(std::cin, command)) {
        splitCommand = splitStr(command);

        ifcommand("uci")
        {
            // UCI identification info
            // std::cout << "id name MyChessEngine\n";
            // std::cout << "id author ns8\n";

            std::cout << "uciok\n"; // UCI approval
        }

        elifcommand("isready")
        {
            std::cout << "readyok\n"; // Engine is ready
        }

        elifsplitcommand(0, "position")
        {
            ifsplitcommand(1, "startpos")
            {
                if (splitCommand.size() == 2) {
                    engine.loadFEN(splitStr(STARTING_FEN));
                }
                else if (splitCommand.size() > 2) {
                    engine.makeUCIMove(splitCommand[splitCommand.size() - 1]);
                    engine.setColor(splitCommand.size() % 2);
                }
            }
        }

        elifsplitcommand(0, "go")
        {
            // Send bestmove (move that will be played by the engine)
            std::string uciMove = engine.getEngineMove();
            engine.makeUCIMove(uciMove);
            std::cout << "bestmove " << uciMove << "\n";
        }

        elifcommand("print")
        {
            printf("\n\nA B C D E F G H\n----------------\n");
            for (int i = 0; i < 64; ++i) {
                int piece = engine.board.mailbox[i];

                if (i % 8 == 0 && i > 0)
                    std::cout << "|" << (i / 8) << "\n";

                std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
            }
            printf("|8\n----------------\nA B C D E F G H\n\n\n");
        }

        elifsplitcommand(0, "move")
        {
            engine.makeUCIMove(splitCommand[1]);

            printf("\n\nA B C D E F G H\n----------------\n");
            for (int i = 0; i < 64; ++i) {
                int piece = engine.board.mailbox[i];

                if (i % 8 == 0 && i > 0)
                    std::cout << "|" << (i / 8) << "\n";

                std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
            }
            printf("|8\n----------------\nA B C D E F G H\n\n\n");
        }

        elifsplitcommand(0, "moves")
        {
            int len = splitCommand.size();
            for (int i = 1; i < len; ++i) {
                engine.makeUCIMove(splitCommand[i]);

                printf("\n\nA B C D E F G H\n----------------\n");
                for (int i = 0; i < 64; ++i) {
                    int piece = engine.board.mailbox[i];

                    if (i % 8 == 0 && i > 0)
                        std::cout << "|" << (i / 8) << "\n";

                    std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
                }
                printf("|8\n----------------\nA B C D E F G H\n\n\n");
            }
        }

        elifcommand("quit")
        {
            // Quit the engine
            std::exit(0);
        }
    }
}
