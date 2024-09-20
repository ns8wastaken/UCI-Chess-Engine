#include "board.hpp"


void Board::precomputeMoves()
{
    /*
         Bitshift offsets

        << 7 | << 8 | << 9
        -----|------|-----
        << 1 |   0  | >> 1
        -----|------|-----
        >> 7 | >> 8 | >> 9
    */

    for (uint64_t i = 0; i < 64; ++i) {
        Bitboard position = (1ULL << i);

        // Knight
        {
            this->precomputedMoves.knightMoves[i] |= position << 17 & Utils::BitMaskA;
            this->precomputedMoves.knightMoves[i] |= position << 15 & Utils::BitMaskB;
            this->precomputedMoves.knightMoves[i] |= position << 10 & Utils::BitMaskA2;
            this->precomputedMoves.knightMoves[i] |= position << 6 & Utils::BitMaskB2;
            this->precomputedMoves.knightMoves[i] |= position >> 6 & Utils::BitMaskA2;
            this->precomputedMoves.knightMoves[i] |= position >> 10 & Utils::BitMaskB2;
            this->precomputedMoves.knightMoves[i] |= position >> 15 & Utils::BitMaskA;
            this->precomputedMoves.knightMoves[i] |= position >> 17 & Utils::BitMaskB;
        }


        // King
        {
            this->precomputedMoves.kingMoves[i] |= position << 9 & Utils::BitMaskA;
            this->precomputedMoves.kingMoves[i] |= position << 8;
            this->precomputedMoves.kingMoves[i] |= position << 7 & Utils::BitMaskB;
            this->precomputedMoves.kingMoves[i] |= position << 1 & Utils::BitMaskA;
            this->precomputedMoves.kingMoves[i] |= position >> 1 & Utils::BitMaskB;
            this->precomputedMoves.kingMoves[i] |= position >> 7 & Utils::BitMaskA;
            this->precomputedMoves.kingMoves[i] |= position >> 8;
            this->precomputedMoves.kingMoves[i] |= position >> 9 & Utils::BitMaskB;
        }
    }
}
