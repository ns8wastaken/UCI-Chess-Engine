#include "engine.hpp"
#include "board.cpp"


Engine::Engine()
{
    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);

    board.precomputeMoves();
}


void Engine::setColor(bool isWhite)
{
    isWhiteTurn = isWhite;

    if (isWhite) {
        ownPiece.PAWN   = Pieces::Piece::W_PAWN;
        ownPiece.KNIGHT = Pieces::Piece::W_KNIGHT;
        ownPiece.BISHOP = Pieces::Piece::W_BISHOP;
        ownPiece.ROOK   = Pieces::Piece::W_ROOK;
        ownPiece.QUEEN  = Pieces::Piece::W_QUEEN;
        ownPiece.KING   = Pieces::Piece::W_KING;

        enemyPiece.PAWN   = Pieces::Piece::B_PAWN;
        enemyPiece.KNIGHT = Pieces::Piece::B_KNIGHT;
        enemyPiece.BISHOP = Pieces::Piece::B_BISHOP;
        enemyPiece.ROOK   = Pieces::Piece::B_ROOK;
        enemyPiece.QUEEN  = Pieces::Piece::B_QUEEN;
        enemyPiece.KING   = Pieces::Piece::B_KING;
    }
    else {
        ownPiece.PAWN   = Pieces::Piece::B_PAWN;
        ownPiece.KNIGHT = Pieces::Piece::B_KNIGHT;
        ownPiece.BISHOP = Pieces::Piece::B_BISHOP;
        ownPiece.ROOK   = Pieces::Piece::B_ROOK;
        ownPiece.QUEEN  = Pieces::Piece::B_QUEEN;
        ownPiece.KING   = Pieces::Piece::B_KING;

        enemyPiece.PAWN   = Pieces::Piece::W_PAWN;
        enemyPiece.KNIGHT = Pieces::Piece::W_KNIGHT;
        enemyPiece.BISHOP = Pieces::Piece::W_BISHOP;
        enemyPiece.ROOK   = Pieces::Piece::W_ROOK;
        enemyPiece.QUEEN  = Pieces::Piece::W_QUEEN;
        enemyPiece.KING   = Pieces::Piece::W_KING;
    }
}


void Engine::loadFEN(const std::vector<std::string>& FEN)
{
    int square = 56; // Start from the top-left corner (a8)

    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);

    board.castlingRights.whiteKingside  = true;
    board.castlingRights.whiteQueenside = true;
    board.castlingRights.blackKingside  = true;
    board.castlingRights.blackQueenside = true;


    for (const char& c : FEN[0]) {
        if (c == '/') {
            square -= 16; // Move to the start of the next rank
        }
        else if (isdigit(c)) {
            square += (c - '0');
        }
        else {
            int piece = Pieces::getPieceFromChar(c);

            board.bitboards[piece] |= (1ULL << square);
            board.mailbox[square] = piece;

            if (Utils::isPieceWhite(piece))
                board.w_occupiedSquares |= (1ULL << square);
            else
                board.b_occupiedSquares |= (1ULL << square);

            ++square;
        }
    }


    if (FEN[1] == "w")
        isWhiteTurn = true;
    else
        isWhiteTurn = false;


    for (const char& c : FEN[2]) {
        switch (c) {
            case 'K': board.castlingRights.whiteKingside = true; break;
            case 'Q': board.castlingRights.whiteQueenside = true; break;
            case 'k': board.castlingRights.blackKingside = true; break;
            case 'q': board.castlingRights.blackQueenside = true; break;
        }
    }

    if (FEN[3] != "-")
        board.enPassantSquare = Utils::squareFromUCI(FEN[3]);
}


Bitboard Engine::generatePieceMoves(const int square, const int piece)
{
    const Bitboard occupiedSquaresAll = (board.w_occupiedSquares | board.b_occupiedSquares);

    Bitboard position = (1ULL << square);


    /*
         Bitshift offsets

        << 7 | << 8 | << 9
        -----|------|-----
        << 1 |   0  | >> 1
        -----|------|-----
        >> 7 | >> 8 | >> 9
    */


    switch (piece) {
        case Pieces::Piece::W_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position << 8) & ~occupiedSquaresAll;
            moves |= (position << 7) & Utils::BitMaskA & board.b_occupiedSquares;
            moves |= (position << 9) & Utils::BitMaskB & board.b_occupiedSquares;

            // Double push if possible
            if ((position & Utils::W_PawnStart) && ((position << 8) & ~occupiedSquaresAll))
                moves |= (position << 16) & ~occupiedSquaresAll;

            return moves;
        }
        case Pieces::Piece::B_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position >> 8) & ~occupiedSquaresAll;
            moves |= (position >> 7) & Utils::BitMaskA & board.w_occupiedSquares;
            moves |= (position >> 9) & Utils::BitMaskB & board.w_occupiedSquares;

            // Double push if possible
            if ((position & Utils::B_PawnStart) && ((position >> 8) & ~occupiedSquaresAll))
                moves |= (position >> 16) & ~occupiedSquaresAll;

            return moves;
        }


        case Pieces::Piece::W_KNIGHT:
        case Pieces::Piece::B_KNIGHT: {
            Bitboard moves = board.precomputedMoves.knightMoves[square];
            return moves & ~(Utils::isPieceWhite(piece) ? board.w_occupiedSquares : board.b_occupiedSquares);
        }


        case Pieces::Piece::W_BISHOP:
        case Pieces::Piece::B_BISHOP: {
            int distLeft  = 7 - (square % 8);
            int distRight = square % 8;
            int distUp    = 7 - (square / 8);
            int distDown  = square / 8;

            int shifts[] = {9, -9, 7, -7};

            int maxLengths[] = {
                std::min(distLeft, distUp),    // Top left
                std::min(distRight, distDown), // Bottom right
                std::min(distRight, distUp),   // Top right
                std::min(distLeft, distDown)   // Bottom left
            };

            Bitboard moves = 0ULL;

            for (int i = 0; i < 4; ++i) {
                for (int j = 1; j <= maxLengths[i]; ++j) {
                    Bitboard result = Utils::BitShift(position, shifts[i] * j);
                    moves |= result;

                    if (result & occupiedSquaresAll)
                        break;
                }
            }

            return moves & ~(Utils::isPieceWhite(piece) ? board.w_occupiedSquares : board.b_occupiedSquares);
        }


        case Pieces::Piece::W_ROOK:
        case Pieces::Piece::B_ROOK: {
            int shifts[] = {1, -1, 8, -8};

            int maxLengths[] = {
                7 - (square % 8),
                square % 8,
                7 - (square / 8),
                square / 8
            };

            Bitboard moves = 0ULL;

            for (int i = 0; i < 4; ++i) {
                for (int j = 1; j <= maxLengths[i]; ++j) {
                    Bitboard result = Utils::BitShift(position, shifts[i] * j);
                    moves |= result;

                    if (result & occupiedSquaresAll)
                        break;
                }
            }

            return moves & ~(Utils::isPieceWhite(piece) ? board.w_occupiedSquares : board.b_occupiedSquares);
        }


        case Pieces::Piece::W_QUEEN:
        case Pieces::Piece::B_QUEEN: {
            int distLeft  = 7 - (square % 8);
            int distRight = square % 8;
            int distUp    = 7 - (square / 8);
            int distDown  = square / 8;

            int shifts[] = {1, -1, 8, -8, 9, -9, 7, -7};

            int maxLengths[] = {
                distLeft,
                distRight,
                distUp,
                distDown,
                std::min(distLeft, distUp),    // Top left
                std::min(distRight, distDown), // Bottom right
                std::min(distRight, distUp),   // Top right
                std::min(distLeft, distDown)   // Bottom left
            };

            Bitboard moves = 0ULL;

            for (int i = 0; i < 8; ++i) {
                for (int j = 1; j <= maxLengths[i]; ++j) {
                    Bitboard result = Utils::BitShift(position, shifts[i] * j);
                    moves |= result;

                    if (result & occupiedSquaresAll)
                        break;
                }
            }

            return moves & ~(Utils::isPieceWhite(piece) ? board.w_occupiedSquares : board.b_occupiedSquares);
        }


        case Pieces::Piece::W_KING: {
            Bitboard moves = board.precomputedMoves.kingMoves[square];

            if (board.castlingRights.whiteKingside) {
                moves |= (position << 2);
            }
            if (board.castlingRights.whiteQueenside) {
                moves |= (position >> 2);
            }

            return moves & ~board.w_occupiedSquares;
        }
        case Pieces::Piece::B_KING: {
            Bitboard moves = board.precomputedMoves.kingMoves[square];

            if (board.castlingRights.blackKingside) {
                moves |= (position << 2);
            }
            if (board.castlingRights.blackQueenside) {
                moves |= (position >> 2);
            }

            return moves & ~board.b_occupiedSquares;
        }
    }

    return 0ULL;
}


MoveList Engine::generateAllMoves()
{
    MoveList botMoves;
    botMoves.moves.fill(Pieces::Move{64, 64, 0});

    for (int i = 0; i < 64; ++i) {
        int piece = board.mailbox[i];

        if (piece == Pieces::Piece::NONE)
            continue;
        else if (isWhiteTurn && !Utils::isPieceWhite(board.mailbox[i])) {
            continue;
        }
        else if (!isWhiteTurn && Utils::isPieceWhite(board.mailbox[i])) {
            continue;
        }

        Bitboard movesBitboard = generatePieceMoves(i, board.mailbox[i]);

        // Loop over piece moves (loop over the bits)
        while (movesBitboard) {
            int offset = std::countr_zero(movesBitboard);
            movesBitboard &= ~(1ULL << offset);

            if ((piece == Pieces::Piece::W_PAWN) && ((1ULL << i) & Utils::B_PawnStart)) {
                botMoves.moves[botMoves.used + 0] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_KNIGHT};
                botMoves.moves[botMoves.used + 1] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_BISHOP};
                botMoves.moves[botMoves.used + 2] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_ROOK};
                botMoves.moves[botMoves.used + 3] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_QUEEN};
                botMoves.used += 4;
            }
            else if ((piece == Pieces::Piece::B_PAWN) && ((1ULL << i) & Utils::W_PawnStart)) {
                botMoves.moves[botMoves.used + 0] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::B_KNIGHT};
                botMoves.moves[botMoves.used + 1] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::B_BISHOP};
                botMoves.moves[botMoves.used + 2] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::B_ROOK};
                botMoves.moves[botMoves.used + 3] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::B_QUEEN};
                botMoves.used += 4;
            }
            else {
                botMoves.moves[botMoves.used] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::NONE};
                ++botMoves.used;
            }
        }
    }

    return botMoves;
}


void Engine::makeMove(const Pieces::Move& move)
{
    // Store history
    board.history.push(Board::HistoryState(board));


    setColor(!isWhiteTurn);
    ++board.plyCount;
    board.moveNumber += isWhiteTurn;


    const int piece = board.mailbox[move.fromSquare];

    const Bitboard fromPos = 1ULL << move.fromSquare;
    const Bitboard toPos   = 1ULL << move.toSquare;


    // Remove castling rights
    if (piece == ownPiece.KING) {
        if (Utils::isPieceWhite(ownPiece.KING)) {
            board.castlingRights.whiteKingside  = false;
            board.castlingRights.whiteQueenside = false;
        }
        else {
            board.castlingRights.blackKingside  = false;
            board.castlingRights.blackQueenside = false;
        }
    }
    else if (piece == ownPiece.ROOK) {
        switch (move.fromSquare) {
            case 0:  board.castlingRights.whiteQueenside = false; break;
            case 7:  board.castlingRights.whiteKingside = false; break;
            case 56: board.castlingRights.blackQueenside = false; break;
            case 63: board.castlingRights.blackKingside = false; break;
        }
    }


    // Handle castling
    if ((piece >> 1 == Pieces::PieceType::KING) && (move.fromSquare + 2 == move.toSquare)) {
        board.bitboards[ownPiece.ROOK] &= ~(1ULL << (move.toSquare + 1));
        board.bitboards[ownPiece.ROOK] |= (1ULL << (move.toSquare - 1));
        board.mailbox[move.toSquare + 1] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare - 1] = ownPiece.ROOK;
    }
    else if ((piece >> 1 == Pieces::PieceType::KING) && (move.fromSquare - 2 == move.toSquare)) {
        board.bitboards[ownPiece.ROOK] &= ~(1ULL << (move.toSquare - 1));
        board.bitboards[ownPiece.ROOK] |= (1ULL << (move.toSquare + 1));
        board.mailbox[move.toSquare - 1] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare + 1] = ownPiece.ROOK;
    }


    // Handle captures (if any)
    if (board.mailbox[move.toSquare] != Pieces::Piece::NONE) {
        int capturedPiece = board.mailbox[move.toSquare];
        board.bitboards[capturedPiece] &= ~toPos; // Remove captured piece from bitboard

        // Remove captured piece from occupied squares
        if (Utils::isPieceWhite(capturedPiece))
            board.w_occupiedSquares &= ~toPos;
        else
            board.b_occupiedSquares &= ~toPos;
    }


    if (move.promotion == Pieces::Piece::NONE) {
        // Normal (no promotions)
        board.bitboards[piece] &= ~fromPos;
        board.bitboards[piece] |= toPos;

        board.mailbox[move.fromSquare] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare]   = piece;
    }
    else {
        // Handle promotions
        board.bitboards[piece] &= ~fromPos;
        board.bitboards[move.promotion] |= toPos;

        board.mailbox[move.fromSquare] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare]   = move.promotion;
    }


    // Update occupied squares
    if (Utils::isPieceWhite(piece)) {
        board.w_occupiedSquares &= ~fromPos;
        board.w_occupiedSquares |= toPos;
    }
    else {
        board.b_occupiedSquares &= ~fromPos;
        board.b_occupiedSquares |= toPos;
    }
}


void Engine::makeUCIMove(const std::string& UCI_Move)
{
    makeMove(Utils::moveFromUCI(UCI_Move));
}


void Engine::undoMove()
{
    const Board::HistoryState& state = board.history.top();

    isWhiteTurn             = !isWhiteTurn;
    board.bitboards         = state.bitboards;
    board.enPassantSquare   = state.enPassantSquare;
    board.castlingRights    = state.castlingRights;
    board.mailbox           = state.mailbox;
    board.w_occupiedSquares = state.w_occupiedSquares;
    board.b_occupiedSquares = state.b_occupiedSquares;

    board.history.pop();
}


bool Engine::isAttacked(const Square square)
{
    // clang-format off
    if      (generatePieceMoves(square, ownPiece.QUEEN)  & board.bitboards[enemyPiece.QUEEN])  {printf("1\n"); return true;}
    else if (generatePieceMoves(square, ownPiece.ROOK)   & board.bitboards[enemyPiece.ROOK])   {printf("2\n"); return true;}
    else if (generatePieceMoves(square, ownPiece.BISHOP) & board.bitboards[enemyPiece.BISHOP]) {printf("3\n"); return true;}
    else if (generatePieceMoves(square, ownPiece.KNIGHT) & board.bitboards[enemyPiece.KNIGHT]) {printf("4\n"); return true;}
    else if (generatePieceMoves(square, ownPiece.PAWN)   & board.bitboards[enemyPiece.PAWN])   {printf("5\n"); return true;}

    return false;
    // clang-format on
}


bool Engine::isLegalMove(const Pieces::Move& move)
{
    Square ownKingSquare = std::countr_zero(board.bitboards[ownPiece.KING]);

    // Castling legality
    if (board.mailbox[move.fromSquare] == ownPiece.KING)
        if (((move.fromSquare + 2 == move.toSquare) && isAttacked(ownKingSquare + 1)) ||
            ((move.fromSquare - 2 == move.toSquare) && isAttacked(ownKingSquare - 1)))
            return false;

    bool isLegal = false;

    makeMove(move);
    if (!isAttacked(ownKingSquare)) {
        // printf("info string nuhuh\n");
        isLegal = true;
    }
    undoMove();

    return isLegal;
}


std::string Engine::getEngineMove()
{
    MoveList botMoves = generateAllMoves();

    Pieces::Move botMove = botMoves.moves[0];

    for (int i = 0; i < botMoves.used; ++i) {
        Pieces::Move move = botMoves.moves[i];

        if (isLegalMove(move)) {
            botMove = move;
            break;
        }
        printf("info string illegal move: %s\n", Utils::toUCI(move).c_str());
    }


    return Utils::toUCI(botMove);
}


uint64_t Engine::perft(const int depth)
{
    if (depth == 0)
        return 1ULL;

    uint64_t nodes = 0;

    MoveList move_list = generateAllMoves();

    for (int i = 0; i < move_list.used; ++i) {
        Pieces::Move move = move_list.moves[i];

        makeMove(move);

        if (!isAttacked(std::countr_zero(board.bitboards[ownPiece.KING])))
            nodes += perft(depth - 1);

        undoMove();
    }

    return nodes;
}


uint64_t Engine::divide(const int depth)
{
    MoveList move_list = generateAllMoves();

    uint64_t totalNodes = 0;

    for (int i = 0; i < move_list.used; ++i) {
        Pieces::Move move = move_list.moves[i];

        makeMove(move);

        if (!isAttacked(std::countr_zero(board.bitboards[ownPiece.KING]))) {
            uint64_t nodes = perft(depth - 1);
            std::cout << Utils::toUCI(move) << ": " << nodes << "\n";
            totalNodes += nodes;
        }

        undoMove();
    }

    return totalNodes;
}
