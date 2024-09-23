#include "board.hpp"


void Board::precomputeMoves()
{
    /*
                 Bitshift offsets


              | <<15 |      | <<17 |
        ------|------|------|------|------
         << 6 | << 7 | << 8 | << 9 | <<10
        ------|------|------|------|------
              | << 1 |   0  | >> 1 |
        ------|------|------|------|------
         >>10 | >> 9 | >> 8 | >> 7 | >> 6
        ------|------|------|------|------
              | >>17 |      | >>15 |
    */

    for (uint64_t i = 0; i < 64; ++i) {
        Bitboard position = (1ULL << i);

        // Knight
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskB) << 17;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskA) << 15;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskB2) << 10;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskA2) << 6;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskB2) >> 6;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskA2) >> 10;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskB) >> 15;
        precomputedMoves.knightMoves[i] |= (position & Utils::BitMaskA) >> 17;


        // King
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskB) << 9;
        precomputedMoves.kingMoves[i] |= (position) << 8;
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskA) << 7;
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskB) << 1;
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskA) >> 1;
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskB) >> 7;
        precomputedMoves.kingMoves[i] |= (position) >> 8;
        precomputedMoves.kingMoves[i] |= (position & Utils::BitMaskA) >> 9;
    }
}
