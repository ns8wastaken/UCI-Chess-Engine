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
        ownPiece.PAWN = Pieces::Piece::W_PAWN;
        ownPiece.KNIGHT = Pieces::Piece::W_KNIGHT;
        ownPiece.BISHOP = Pieces::Piece::W_BISHOP;
        ownPiece.ROOK = Pieces::Piece::W_ROOK;
        ownPiece.QUEEN = Pieces::Piece::W_QUEEN;
        ownPiece.KING = Pieces::Piece::W_KING;

        enemyPiece.PAWN = Pieces::Piece::B_PAWN;
        enemyPiece.KNIGHT = Pieces::Piece::B_KNIGHT;
        enemyPiece.BISHOP = Pieces::Piece::B_BISHOP;
        enemyPiece.ROOK = Pieces::Piece::B_ROOK;
        enemyPiece.QUEEN = Pieces::Piece::B_QUEEN;
        enemyPiece.KING = Pieces::Piece::B_KING;
    }
    else {
        ownPiece.PAWN = Pieces::Piece::B_PAWN;
        ownPiece.KNIGHT = Pieces::Piece::B_KNIGHT;
        ownPiece.BISHOP = Pieces::Piece::B_BISHOP;
        ownPiece.ROOK = Pieces::Piece::B_ROOK;
        ownPiece.QUEEN = Pieces::Piece::B_QUEEN;
        ownPiece.KING = Pieces::Piece::B_KING;

        enemyPiece.PAWN = Pieces::Piece::W_PAWN;
        enemyPiece.KNIGHT = Pieces::Piece::W_KNIGHT;
        enemyPiece.BISHOP = Pieces::Piece::W_BISHOP;
        enemyPiece.ROOK = Pieces::Piece::W_ROOK;
        enemyPiece.QUEEN = Pieces::Piece::W_QUEEN;
        enemyPiece.KING = Pieces::Piece::W_KING;
    }
}


void Engine::loadFEN(const std::vector<std::string>& FEN)
{
    int square = 56; // Start from the top-left corner (a8)

    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);

    board.castlingRights.whiteKingside = true;
    board.castlingRights.whiteQueenside = true;
    board.castlingRights.blackKingside = true;
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
            moves |= (position << 7) & board.b_occupiedSquares & Utils::BitMaskB;
            moves |= (position << 9) & board.b_occupiedSquares & Utils::BitMaskA;

            // Double push if possible
            if ((position & Utils::W_PawnStart) && ((position << 8) & ~occupiedSquaresAll))
                moves |= (position << 16) & ~occupiedSquaresAll;

            return moves;
        }
        case Pieces::Piece::B_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position >> 8) & ~occupiedSquaresAll;
            moves |= (position >> 7) & board.w_occupiedSquares & Utils::BitMaskB;
            moves |= (position >> 9) & board.w_occupiedSquares & Utils::BitMaskA;

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
            int distLeft = 7 - (square % 8);
            int distRight = square % 8;
            int distUp = 7 - (square / 8);
            int distDown = square / 8;

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
            int distLeft = 7 - (square % 8);
            int distRight = square % 8;
            int distUp = 7 - (square / 8);
            int distDown = square / 8;

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


        case Pieces::Piece::W_KING:
        case Pieces::Piece::B_KING: {
            Bitboard moves = board.precomputedMoves.kingMoves[square];
            return moves & ~(Utils::isPieceWhite(piece) ? board.w_occupiedSquares : board.b_occupiedSquares);
        }
    }

    return 0ULL;
}


std::array<Pieces::Move, 218ULL> Engine::generateAllMoves()
{
    std::array<Pieces::Move, 218ULL> botMoves;
    size_t botMovesUsed = 0ULL;
    botMoves.fill(Pieces::Move{64, 64, 0});

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
            int offset = __builtin_ctzll(movesBitboard);
            movesBitboard &= ~(1ULL << offset);

            if ((piece == Pieces::Piece::W_PAWN) && ((1ULL << i) & Utils::B_PawnStart)) {
                botMoves[botMovesUsed + 0] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_W_KNIGHT};
                botMoves[botMovesUsed + 1] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_W_BISHOP};
                botMoves[botMovesUsed + 2] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_W_ROOK};
                botMoves[botMovesUsed + 3] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_W_QUEEN};
                botMovesUsed += 4;
            }
            else if ((piece == Pieces::Piece::B_PAWN) && ((1ULL << i) & Utils::W_PawnStart)) {
                botMoves[botMovesUsed + 0] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_B_KNIGHT};
                botMoves[botMovesUsed + 1] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_B_BISHOP};
                botMoves[botMovesUsed + 2] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_B_ROOK};
                botMoves[botMovesUsed + 3] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Promotion::P_B_QUEEN};
                botMovesUsed += 4;
            }
            else {
                botMoves[botMovesUsed] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), 0};
                ++botMovesUsed;
            }
        }
    }

    return botMoves;
}


bool Engine::isAttacked(const Square square)
{
    // Print threats ===============
    // std::string set = std::bitset<64>(generatePieceMoves(square, bishop) | generatePieceMoves(square, rook) | generatePieceMoves(square, queen) | generatePieceMoves(square, knight) | generatePieceMoves(square, pawn)).to_string();

    // for (int i = 0; i < 64; ++i) {
    //     if (i % 8 == 0) printf("\n");
    //     if (set[i] == '1')
    //         printf("# ");
    //     else
    //         printf(". ");
    // }
    // printf("\n");
    // ==============================

    if (generatePieceMoves(square, ownPiece.BISHOP) & board.bitboards[enemyPiece.BISHOP]) return true;
    else if (generatePieceMoves(square, ownPiece.ROOK) & board.bitboards[enemyPiece.ROOK]) return true;
    else if (generatePieceMoves(square, ownPiece.QUEEN) & board.bitboards[enemyPiece.QUEEN]) return true;
    else if (generatePieceMoves(square, ownPiece.KNIGHT) & board.bitboards[enemyPiece.KNIGHT]) return true;
    else if (generatePieceMoves(square, ownPiece.PAWN) & board.bitboards[enemyPiece.PAWN]) return true;

    // printf("pasdjfoiajsdfojaposdfjoasdf\n");

    return false;
}


void Engine::makeMove(const Pieces::Move& move)
{
    board.history.push(Board::HistoryState{
        .bitboards = board.bitboards,
        .enPassantSquare = board.enPassantSquare,
        .castlingRights = board.castlingRights,
        .mailbox = board.mailbox
    });

    int piece = board.mailbox[move.fromSquare];

    // Handle captures (if any)
    if (board.mailbox[move.toSquare] != Pieces::Piece::NONE) {
        int capturedPiece = board.mailbox[move.toSquare];
        board.bitboards[capturedPiece] &= ~(1ULL << move.toSquare); // Remove captured piece from bitboard

        // Remove captured piece from occupied squares
        if (Utils::isPieceWhite(capturedPiece))
            board.w_occupiedSquares &= ~(1ULL << move.toSquare);
        else
            board.b_occupiedSquares &= ~(1ULL << move.toSquare);
    }

    // Update bitboard
    board.bitboards[piece] &= ~(1ULL << move.fromSquare);
    board.bitboards[piece] |= (1ULL << move.toSquare);

    // Update mailbox
    board.mailbox[move.fromSquare] = Pieces::Piece::NONE;
    board.mailbox[move.toSquare] = piece;

    // Update occupied squares
    if (Utils::isPieceWhite(piece)) {
        board.w_occupiedSquares &= ~(1ULL << move.fromSquare);
        board.w_occupiedSquares |= (1ULL << move.toSquare);
    }
    else {
        board.b_occupiedSquares &= ~(1ULL << move.fromSquare);
        board.b_occupiedSquares |= (1ULL << move.toSquare);
    }
}


void Engine::makeUCIMove(const std::string& UCI_Move)
{
    makeMove(Utils::moveFromUCI(UCI_Move));
}


void Engine::undoMove()
{
    const Board::HistoryState& state = board.history.top();

    board.bitboards = state.bitboards;
    board.enPassantSquare = state.enPassantSquare;
    board.castlingRights = state.castlingRights;
    board.mailbox = state.mailbox;

    board.history.pop();
}


bool Engine::isLegalMove(const Pieces::Move& move)
{
    bool isLegal = true;

    int king = (isWhiteTurn ? Pieces::Piece::W_KING : Pieces::Piece::B_KING);

    makeMove(move);
    if (isAttacked(__builtin_ctzll(board.bitboards[king]))) {
        printf("info string nuhuh\n");
        isLegal = false;
    }
    undoMove();

    return isLegal;
}


std::string Engine::getEngineMove()
{
    std::array<Pieces::Move, 218ULL> botMoves = generateAllMoves();

    Pieces::Move botMove = botMoves[0];

    for (const Pieces::Move& move : botMoves) {
        if (isLegalMove(move)) {
            botMove = move;
            break;
        }
        printf("info string illegal move: %s\n", Utils::toUCI(move).c_str());
    }

    // if (isAttacked(__builtin_ctzll(board.bitboards[isWhiteTurn ? Pieces::Piece::W_KING : Pieces::Piece::B_KING]))) {
    //     printf("info string nuhuh !!!\n");
    // }


    return Utils::toUCI(botMove);
}
