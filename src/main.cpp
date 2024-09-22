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


const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


void printBitboard(uint64_t bitboard)
{
    std::string set = std::bitset<64>(bitboard).to_string();
    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) std::cout << "\n";
        std::cout << (((int)set[i] - 48) ? "# " : ". ");
    }
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
                    engine.setColor(true);
                }
                else if (splitCommand.size() > 2) {
                    engine.makeUCIMove(splitCommand[splitCommand.size() - 1]);
                }
            }

            elifsplitcommand(1, "fen")
            {
                if (splitCommand.size() == 8) {
                    std::vector<std::string> fen = {
                        splitCommand[2],
                        splitCommand[3],
                        splitCommand[4],
                        splitCommand[5],
                        splitCommand[6],
                        splitCommand[7],
                    };
                    engine.loadFEN(fen);
                }
                else if (splitCommand.size() > 8) {
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

        elifsplitcommand(0, "perft")
        {
            const uint64_t nodes = engine.perft(std::stoi(splitCommand[1]));

            std::cout << nodes << "\n";
        }

        elifsplitcommand(0, "divide")
        {
            const uint64_t nodes = engine.divide(std::stoi(splitCommand[1]));

            std::cout << "Total nodes: " << nodes << "\n";
        }

        elifcommand("iswhite")
        {
            std::cout << "Is white: " << engine.isWhiteTurn << "\n";
        }

        elifcommand("get moves")
        {
            int count = 0;

            MoveList moves = engine.generateAllMoves();

            for (int i = 0; i < moves.used; ++i) {
                Pieces::Move move = moves.moves[i];

                if (engine.isLegalMove(move)) {
                    std::cout << Utils::toUCI(move) << "\n";
                    ++count;
                }
            }
            std::cout << "Total move count: " << count << "\n";
        }

        elifcommand("print")
        {
            printf("\n\nA B C D E F G H\n----------------\n");
            for (int i = 63; i >= 0; --i) {
                int row   = i / 8;
                int col   = i % 8;
                int piece = engine.board.mailbox[row * 8 + (7 - col)];

                if (i % 8 == 7 && i < 63)
                    std::cout << "|" << (row + 2) << "\n";

                std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
            }
            printf("|1\n----------------\nA B C D E F G H\n\n\n");

            for (int i = 0; i < Pieces::Piece::PIECE_COUNT; ++i) {
                printf("Piece: %c\n", Pieces::getPieceChar(i));
                printBitboard(engine.board.bitboards[i]);
                printf("\n\n");
            }
        }

        elifsplitcommand(0, "move")
        {
            engine.makeUCIMove(splitCommand[1]);

            printf("\n\nA B C D E F G H\n----------------\n");
            for (int i = 63; i >= 0; --i) {
                int row   = i / 8;
                int col   = i % 8;
                int piece = engine.board.mailbox[row * 8 + (7 - col)];

                if (i % 8 == 7 && i < 63)
                    std::cout << "|" << (row + 2) << "\n";

                std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
            }
            printf("|1\n----------------\nA B C D E F G H\n\n\n");
        }

        elifsplitcommand(0, "moves")
        {
            int len = splitCommand.size();
            for (int i = 1; i < len; ++i) {
                engine.makeUCIMove(splitCommand[i]);

                std::cout << splitCommand[i] << "\n";

                printf("\n\nA B C D E F G H\n----------------\n");
                for (int i = 63; i >= 0; --i) {
                    int row   = i / 8;
                    int col   = i % 8;
                    int piece = engine.board.mailbox[row * 8 + (7 - col)];

                    if (i % 8 == 7 && i < 63)
                        std::cout << "|" << (row + 2) << "\n";

                    std::cout << ((piece == Pieces::Piece::NONE) ? ' ' : Pieces::getPieceChar(piece)) << " ";
                }
                printf("|1\n----------------\nA B C D E F G H\n\n\n");
            }
        }

        elifcommand("undo")
        {
            engine.undoMove();
        }

        elifcommand("quit")
        {
            // Quit the engine
            std::exit(0);
        }

        else
        {
            std::cout << command << "\n";
        }
    }
}
