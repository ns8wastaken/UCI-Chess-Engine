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


void Engine::flipColor()
{
    isWhiteTurn = !isWhiteTurn;

    ownPiece.PAWN ^= 1;
    ownPiece.KNIGHT ^= 1;
    ownPiece.BISHOP ^= 1;
    ownPiece.ROOK ^= 1;
    ownPiece.QUEEN ^= 1;
    ownPiece.KING ^= 1;

    enemyPiece.PAWN ^= 1;
    enemyPiece.KNIGHT ^= 1;
    enemyPiece.BISHOP ^= 1;
    enemyPiece.ROOK ^= 1;
    enemyPiece.QUEEN ^= 1;
    enemyPiece.KING ^= 1;
}


void Engine::loadFEN(const std::vector<std::string>& FEN)
{
    int square = 56; // Start from the top-left corner (a8)

    board.bitboards.fill(0ULL);
    board.mailbox.fill(Pieces::Piece::NONE);

    board.occupiedSquares[0] = 0ULL;
    board.occupiedSquares[1] = 0ULL;


    board.castlingFlags = 0;


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

            board.occupiedSquares[Utils::isPieceWhite(piece)] |= (1ULL << square);

            ++square;
        }
    }


    if (FEN[1] == "w")
        setColor(true);
    else
        setColor(false);


    for (const char& c : FEN[2]) {
        switch (c) {
            case 'K': board.castlingFlags |= Utils::CastlingRightsFlags::W_KINGSIDE; break;
            case 'Q': board.castlingFlags |= Utils::CastlingRightsFlags::W_QUEENSIDE; break;
            case 'k': board.castlingFlags |= Utils::CastlingRightsFlags::B_KINGSIDE; break;
            case 'q': board.castlingFlags |= Utils::CastlingRightsFlags::B_QUEENSIDE; break;
        }
    }

    if (FEN[3] != "-")
        board.enPassantSquare = Utils::squareFromUCI(FEN[3]);
}


Bitboard Engine::generatePieceMoves(const int square, const int piece)
{
    const Bitboard occupiedSquaresAll = (board.occupiedSquares[0] | board.occupiedSquares[1]);

    Bitboard position = (1ULL << square);

    Bitboard enPassantPos = (1ULL << board.enPassantSquare);
    if (board.enPassantSquare == 64) {
        enPassantPos = 0ULL;
    }


    /*
         Bitshift offsets

        << 7 | << 8 | << 9
        -----|------|-----
        << 1 |   0  | >> 1
        -----|------|-----
        >> 9 | >> 8 | >> 7
    */


    switch (piece) {
        case Pieces::Piece::W_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position << 8) & ~occupiedSquaresAll;
            moves |= (position << 7) & Utils::BitMaskB & (board.occupiedSquares[0] | enPassantPos);
            moves |= (position << 9) & Utils::BitMaskA & (board.occupiedSquares[0] | enPassantPos);

            // Double push if possible
            if ((position & Utils::W_PawnStart) && ((position << 8) & ~occupiedSquaresAll))
                moves |= (position << 16) & ~occupiedSquaresAll;

            return moves;
        }
        case Pieces::Piece::B_PAWN: {
            Bitboard moves = 0ULL;

            moves |= (position >> 8) & ~occupiedSquaresAll;
            moves |= (position >> 7) & Utils::BitMaskA & (board.occupiedSquares[1] | enPassantPos);
            moves |= (position >> 9) & Utils::BitMaskB & (board.occupiedSquares[1] | enPassantPos);

            // Double push if possible
            if ((position & Utils::B_PawnStart) && ((position >> 8) & ~occupiedSquaresAll))
                moves |= (position >> 16) & ~occupiedSquaresAll;

            return moves;
        }


        case Pieces::Piece::W_KNIGHT:
        case Pieces::Piece::B_KNIGHT: {
            Bitboard moves = board.precomputedMoves.knightMoves[square];
            return moves & ~board.occupiedSquares[Utils::isPieceWhite(piece)];
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

            return moves & ~board.occupiedSquares[Utils::isPieceWhite(piece)];
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

            return moves & ~board.occupiedSquares[Utils::isPieceWhite(piece)];
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

            return moves & ~board.occupiedSquares[Utils::isPieceWhite(piece)];
        }


        case Pieces::Piece::W_KING: {
            Bitboard moves = board.precomputedMoves.kingMoves[square];

            if ((board.castlingFlags & Utils::CastlingRightsFlags::W_KINGSIDE) &&
                board.mailbox[square + 1] == Pieces::Piece::NONE &&
                board.mailbox[square + 2] == Pieces::Piece::NONE) {
                moves |= (position << 2);
            }
            if ((board.castlingFlags & Utils::CastlingRightsFlags::W_QUEENSIDE) &&
                board.mailbox[square - 1] == Pieces::Piece::NONE &&
                board.mailbox[square - 2] == Pieces::Piece::NONE &&
                board.mailbox[square - 3] == Pieces::Piece::NONE) {
                moves |= (position >> 2);
            }

            return moves & ~board.occupiedSquares[1];
        }
        case Pieces::Piece::B_KING: {
            Bitboard moves = board.precomputedMoves.kingMoves[square];

            if ((board.castlingFlags & Utils::CastlingRightsFlags::B_KINGSIDE) &&
                board.mailbox[square + 1] == Pieces::Piece::NONE &&
                board.mailbox[square + 2] == Pieces::Piece::NONE) {
                moves |= (position << 2);
            }
            if ((board.castlingFlags & Utils::CastlingRightsFlags::B_QUEENSIDE) &&
                board.mailbox[square - 1] == Pieces::Piece::NONE &&
                board.mailbox[square - 2] == Pieces::Piece::NONE &&
                board.mailbox[square - 3] == Pieces::Piece::NONE) {
                moves |= (position >> 2);
            }

            return moves & ~board.occupiedSquares[0];
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
                botMoves.moves[botMoves.used]     = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_KNIGHT};
                botMoves.moves[botMoves.used + 1] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_BISHOP};
                botMoves.moves[botMoves.used + 2] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_ROOK};
                botMoves.moves[botMoves.used + 3] = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::W_QUEEN};
                botMoves.used += 4;
            }
            else if ((piece == Pieces::Piece::B_PAWN) && ((1ULL << i) & Utils::W_PawnStart)) {
                botMoves.moves[botMoves.used]     = Pieces::Move{s_cast(uint8_t, i), s_cast(uint8_t, offset), Pieces::Piece::B_KNIGHT};
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
    board.history.history[board.history.used++] = Board::HistoryState(board);


    ++board.plyCount;


    const int piece = board.mailbox[move.fromSquare];

    const Bitboard fromPos = 1ULL << move.fromSquare;
    const Bitboard toPos   = 1ULL << move.toSquare;


    // Remove castling rights
    if (piece == ownPiece.KING) {
        if (Utils::isPieceWhite(ownPiece.KING)) {
            board.castlingFlags &= ~Utils::CastlingRightsFlags::W_KINGSIDE;
            board.castlingFlags &= ~Utils::CastlingRightsFlags::W_QUEENSIDE;
        }
        else {
            board.castlingFlags &= ~Utils::CastlingRightsFlags::B_KINGSIDE;
            board.castlingFlags &= ~Utils::CastlingRightsFlags::B_QUEENSIDE;
        }
    }
    else {
        switch (move.fromSquare) {
            case 0:  board.castlingFlags &= ~Utils::CastlingRightsFlags::W_QUEENSIDE; break;
            case 7:  board.castlingFlags &= ~Utils::CastlingRightsFlags::W_KINGSIDE; break;
            case 56: board.castlingFlags &= ~Utils::CastlingRightsFlags::B_QUEENSIDE; break;
            case 63: board.castlingFlags &= ~Utils::CastlingRightsFlags::B_KINGSIDE; break;
        }
        switch (move.toSquare) {
            case 0:  board.castlingFlags &= ~Utils::CastlingRightsFlags::W_QUEENSIDE; break;
            case 7:  board.castlingFlags &= ~Utils::CastlingRightsFlags::W_KINGSIDE; break;
            case 56: board.castlingFlags &= ~Utils::CastlingRightsFlags::B_QUEENSIDE; break;
            case 63: board.castlingFlags &= ~Utils::CastlingRightsFlags::B_KINGSIDE; break;
        }
    }


    // Handle castling
    if ((piece >> 1 == Pieces::PieceType::KING) && (move.fromSquare + 2 == move.toSquare)) {
        // Kingside castle
        board.bitboards[ownPiece.ROOK] &= ~(toPos << 1);
        board.bitboards[ownPiece.ROOK] |= (toPos >> 1);

        board.mailbox[move.toSquare + 1] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare - 1] = ownPiece.ROOK;

        board.occupiedSquares[Utils::isPieceWhite(piece)] &= ~(toPos << 1);
        board.occupiedSquares[Utils::isPieceWhite(piece)] |= (toPos >> 1);
    }
    else if ((piece >> 1 == Pieces::PieceType::KING) && (move.fromSquare - 2 == move.toSquare)) {
        // Queenside castle
        board.bitboards[ownPiece.ROOK] &= ~(toPos >> 1);
        board.bitboards[ownPiece.ROOK] |= (toPos << 1);

        board.mailbox[move.toSquare - 2] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare + 1] = ownPiece.ROOK;

        board.occupiedSquares[Utils::isPieceWhite(piece)] &= ~(toPos >> 2);
        board.occupiedSquares[Utils::isPieceWhite(piece)] |= (toPos << 1);
    }


    // Handle captures (if any)
    if (board.mailbox[move.toSquare] != Pieces::Piece::NONE) {
        int capturedPiece = board.mailbox[move.toSquare];
        // Remove captured piece from bitboard
        board.bitboards[capturedPiece] &= ~toPos;

        // Remove captured piece from occupied squares
        board.occupiedSquares[Utils::isPieceWhite(capturedPiece)] &= ~toPos;
    }
    else if ((piece >> 1) == Pieces::PieceType::PAWN && (move.toSquare == board.enPassantSquare)) {
        if (Utils::isPieceWhite(piece)) {
            board.bitboards[enemyPiece.PAWN] &= ~(toPos >> 8);
            board.occupiedSquares[0] &= ~(toPos >> 8);
            board.mailbox[move.toSquare - 8] = Pieces::Piece::NONE;
        }
        else {
            board.bitboards[enemyPiece.PAWN] &= ~(toPos << 8);
            board.occupiedSquares[1] &= ~(toPos << 8);
            board.mailbox[move.toSquare + 8] = Pieces::Piece::NONE;
        }
    }


    // Update bitboards
    if (move.promotionPiece == Pieces::Piece::NONE) {
        // Normal (no promotions)
        board.bitboards[piece] &= ~fromPos;
        board.bitboards[piece] |= toPos;

        board.mailbox[move.fromSquare] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare]   = piece;
    }
    else {
        // Handle promotions
        board.bitboards[piece] &= ~fromPos;

        board.bitboards[move.promotionPiece] |= toPos;

        board.mailbox[move.fromSquare] = Pieces::Piece::NONE;
        board.mailbox[move.toSquare]   = move.promotionPiece;
    }


    // Update occupied squares
    board.occupiedSquares[Utils::isPieceWhite(piece)] &= ~fromPos;
    board.occupiedSquares[Utils::isPieceWhite(piece)] |= toPos;


    flipColor();

    // Remove en passant square
    board.enPassantSquare = 64;


    // Set en passant square for next turn
    if ((board.mailbox[move.toSquare] == Pieces::Piece::W_PAWN) && (move.fromSquare + 16 == move.toSquare)) {
        board.enPassantSquare = move.fromSquare + 8;
    }
    else if ((board.mailbox[move.toSquare] == Pieces::Piece::B_PAWN) && (move.fromSquare - 16 == move.toSquare)) {
        board.enPassantSquare = move.fromSquare - 8;
    }
}


void Engine::makeUCIMove(const std::string& UCI_Move)
{
    Pieces::Move move = Utils::moveFromUCI(UCI_Move);
    makeMove(move);
}


void Engine::undoMove()
{
    const Board::HistoryState state = board.history.history[--board.history.used];

    flipColor();

    board.bitboards          = state.bitboards;
    board.mailbox            = state.mailbox;
    board.enPassantSquare    = state.enPassantSquare;
    board.castlingFlags      = state.castlingFlags;
    board.occupiedSquares[0] = state.occupiedSquares[0];
    board.occupiedSquares[1] = state.occupiedSquares[1];

    --board.plyCount;
}


bool Engine::isAttacked(const Square square)
{
    // clang-format off
    if      (generatePieceMoves(square, ownPiece.QUEEN)  & board.bitboards[enemyPiece.QUEEN])  return true;
    else if (generatePieceMoves(square, ownPiece.ROOK)   & board.bitboards[enemyPiece.ROOK])   return true;
    else if (generatePieceMoves(square, ownPiece.BISHOP) & board.bitboards[enemyPiece.BISHOP]) return true;
    else if (generatePieceMoves(square, ownPiece.KNIGHT) & board.bitboards[enemyPiece.KNIGHT]) return true;
    else if (generatePieceMoves(square, ownPiece.PAWN)   & board.bitboards[enemyPiece.PAWN])   return true;

    return false;
    // clang-format on
}


bool Engine::isLegalMove(const Pieces::Move& move)
{
    Square ownKingSquare = std::countr_zero(board.bitboards[ownPiece.KING]);

    // Castling legality
    if (board.mailbox[move.fromSquare] == ownPiece.KING && ((move.toSquare == move.fromSquare + 2) || (move.toSquare == move.fromSquare - 2))) {
        if (isAttacked(ownKingSquare) ||
            ((move.fromSquare + 2 == move.toSquare) && (isAttacked(ownKingSquare + 1) || isAttacked(ownKingSquare + 2))) ||
            ((move.fromSquare - 2 == move.toSquare) && (isAttacked(ownKingSquare - 1) || isAttacked(ownKingSquare - 2)))) {
            return false;
        }
        else {
            return true;
        }
    }

    makeMove(move);
    flipColor();

    bool isLegal = !isAttacked(std::countr_zero(board.bitboards[ownPiece.KING]));

    flipColor();
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
    }

    makeMove(botMove);

    return Utils::toUCI(botMove);
}
