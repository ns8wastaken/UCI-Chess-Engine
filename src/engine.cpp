#include "engine.hpp"
#include "board.cpp"


Engine::Engine()
{
    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);
}


void Engine::loadFEN(const std::string& FEN)
{
    int square = 56; // Start from the top-left corner (a8)

    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);

    for (const char& c : FEN) {
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
}


Bitboard Engine::generatePieceMoves(int square)
{
    const Bitboard occupiedSquaresAll = (board.w_occupiedSquares | board.b_occupiedSquares);

    int piece = board.mailbox[square];

    Bitboard position = (1ULL << square);

    switch (piece) {
        case Pieces::Piece::W_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position << 8) & ~occupiedSquaresAll;
            // moves |= (position >> 7) & board.b_occupiedSquares & Utils::BitMaskA;
            // moves |= (position >> 9) & board.b_occupiedSquares & Utils::BitMaskB;

            // // Double push if needed
            // if ((position & Utils::W_PawnStart) && ((position >> 8) & ~occupiedSquaresAll))
            //     moves |= (position >> 16) & ~occupiedSquaresAll;

            return moves;
        }
        case Pieces::Piece::B_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position >> 8) & ~occupiedSquaresAll;
            // moves |= (position << 7) & board.w_occupiedSquares & Utils::BitMaskB;
            // moves |= (position << 9) & board.w_occupiedSquares & Utils::BitMaskA;

            // // Double push if needed
            // if ((position & Utils::B_PawnStart) && ((position << 8) & ~occupiedSquaresAll))
            //     moves |= (position << 16) & ~occupiedSquaresAll;

            return moves;
        }


        // case Pieces::Piece::W_KNIGHT:
        // case Pieces::Piece::B_KNIGHT: {
        //     Bitboard moves = board.precomputedMoves.knightMoves[square];
        //     return moves & ~(Utils::isPieceWhite(piece) ? board.w_pieces : board.b_pieces);
        // }


        // case Pieces::Piece::W_BISHOP:
        // case Pieces::Piece::B_BISHOP: {
        //     int zeros = __builtin_ctzll(position);

        //     int distLeft = 7 - (zeros % 8);
        //     int distRight = zeros % 8;
        //     int distUp = 7 - (zeros / 8);
        //     int distDown = zeros / 8;

        //     int shifts[] = {9, -9, 7, -7};

        //     int maxLengths[] = {
        //         std::min(distLeft, distUp),    // Top left
        //         std::min(distRight, distDown), // Bottom right
        //         std::min(distRight, distUp),   // Top right
        //         std::min(distLeft, distDown)   // Bottom left
        //     };

        //     Bitboard moves = 0ULL;

        //     for (int i = 0; i < 4; ++i) {
        //         for (int j = 1; j <= maxLengths[i]; ++j) {
        //             Bitboard result = Utils::BitShift(position, shifts[i] * j);
        //             moves |= result;

        //             if (result & occupiedSquaresAll)
        //                 break;
        //         }
        //     }

        //     return moves & ~(Utils::isPieceWhite(piece) ? board.w_pieces : board.b_pieces);
        // }


        // case Pieces::Piece::W_ROOK:
        // case Pieces::Piece::B_ROOK: {
        //     int zeros = __builtin_ctzll(position);

        //     int shifts[] = {1, -1, 8, -8};

        //     int maxLengths[] = {
        //         7 - (zeros % 8),
        //         zeros % 8,
        //         7 - (zeros / 8),
        //         zeros / 8
        //     };

        //     Bitboard moves = 0ULL;

        //     for (int i = 0; i < 4; ++i) {
        //         for (int j = 1; j <= maxLengths[i]; ++j) {
        //             Bitboard result = Utils::BitShift(position, shifts[i] * j);
        //             moves |= result;

        //             if (result & occupiedSquaresAll)
        //                 break;
        //         }
        //     }

        //     return moves & ~(Utils::isPieceWhite(piece) ? board.w_pieces : board.b_pieces);
        // }


        // case Pieces::Piece::W_QUEEN:
        // case Pieces::Piece::B_QUEEN: {
        //     int zeros = __builtin_ctzll(position);

        //     int distLeft = 7 - (zeros % 8);
        //     int distRight = zeros % 8;
        //     int distUp = 7 - (zeros / 8);
        //     int distDown = zeros / 8;

        //     int shifts[] = {1, -1, 8, -8, 9, -9, 7, -7};

        //     int maxLengths[] = {
        //         distLeft,
        //         distRight,
        //         distUp,
        //         distDown,
        //         std::min(distLeft, distUp),    // Top left
        //         std::min(distRight, distDown), // Bottom right
        //         std::min(distRight, distUp),   // Top right
        //         std::min(distLeft, distDown)   // Bottom left
        //     };

        //     Bitboard moves = 0ULL;

        //     for (int i = 0; i < 8; ++i) {
        //         for (int j = 1; j <= maxLengths[i]; ++j) {
        //             Bitboard result = Utils::BitShift(position, shifts[i] * j);
        //             moves |= result;

        //             if (result & occupiedSquaresAll)
        //                 break;
        //         }
        //     }

        //     return moves & ~(Utils::isPieceWhite(piece) ? board.w_pieces : board.b_pieces);
        // }


        // case Pieces::Piece::W_KING:
        // case Pieces::Piece::B_KING: {
        //     Bitboard moves = board.precomputedMoves.kingMoves[square];
        //     return moves & ~(Utils::isPieceWhite(piece) ? board.w_pieces : board.b_pieces);
        // }
    }

    return 0ULL;
}


std::array<Pieces::Move, 218ULL> Engine::generateAllMoves()
{
    std::array<Pieces::Move, 218ULL> botMoves;
    size_t botMovesUsed = 0ULL;
    botMoves.fill(Pieces::Move{64, 64});

    for (int i = 0; i < 64; ++i) {
        if (board.mailbox[i] == Pieces::Piece::NONE)
            continue;
        else if (isWhiteTurn && !Utils::isPieceWhite(board.mailbox[i])) {
            continue;
        }
        else if (!isWhiteTurn && Utils::isPieceWhite(board.mailbox[i])) {
            continue;
        }

        Bitboard movesBitboard = generatePieceMoves(i);

        // Loop over piece moves (loop over the bits)
        for (int offset = 0; movesBitboard; movesBitboard >>= 1, ++offset) {
            if (movesBitboard & 1) {
                botMoves[botMovesUsed] = Pieces::Move{static_cast<uint8_t>(i), static_cast<uint8_t>(offset)};
                ++botMovesUsed;
            }
        }
    }

    return botMoves;
}


void Engine::makeMove(Pieces::Move move)
{
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


std::string Engine::getEngineMove()
{
    std::array<Pieces::Move, 218ULL> botMoves = generateAllMoves();
    Pieces::Move move = botMoves[0];

    // for (int i = 1; i < 218; ++i) {
    //     if (botMoves[i].fromSquare == 64 || botMoves[i].toSquare == 64)
    //         continue;

    //     if (board.mailbox[botMoves[i].fromSquare] == Pieces::Piece::W_KNIGHT)
    //         printf("[%d] %s\n", i, Utils::toUCI(botMoves[i]).c_str());
    // }

    // int square = 8;
    // printf("%i  %i\n", (bool)(board.bitboards[0] & (1ULL << 8)), board.mailbox[square] == Pieces::Piece::W_PAWN);

    return Utils::toUCI(move);
}


void Engine::makeUCIMove(const std::string& UCI_Move)
{
    makeMove(Utils::fromUCI(UCI_Move));
}
