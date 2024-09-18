#include "board.hpp"


void Board::precomputeMoves()
{
    precomputedMoves.knightMoves.fill(0ULL);
    precomputedMoves.kingMoves.fill(0ULL);


    for (uint64_t i = 0; i < 64; ++i) {
        Bitboard position = (1ULL << i);

        // Knight
        {
            precomputedMoves.knightMoves[i] |= position << 17 & Utils::BitMaskA;
            precomputedMoves.knightMoves[i] |= position << 15 & Utils::BitMaskB;
            precomputedMoves.knightMoves[i] |= position << 10 & Utils::BitMaskA2;
            precomputedMoves.knightMoves[i] |= position << 6 & Utils::BitMaskB2;
            precomputedMoves.knightMoves[i] |= position >> 6 & Utils::BitMaskA2;
            precomputedMoves.knightMoves[i] |= position >> 10 & Utils::BitMaskB2;
            precomputedMoves.knightMoves[i] |= position >> 15 & Utils::BitMaskA;
            precomputedMoves.knightMoves[i] |= position >> 17 & Utils::BitMaskB;
        }


        // King
        {
            precomputedMoves.kingMoves[i] |= position << 9 & Utils::BitMaskA;
            precomputedMoves.kingMoves[i] |= position << 8;
            precomputedMoves.kingMoves[i] |= position << 7 & Utils::BitMaskB;
            precomputedMoves.kingMoves[i] |= position << 1 & Utils::BitMaskA;
            precomputedMoves.kingMoves[i] |= position >> 1 & Utils::BitMaskB;
            precomputedMoves.kingMoves[i] |= position >> 7 & Utils::BitMaskA;
            precomputedMoves.kingMoves[i] |= position >> 8;
            precomputedMoves.kingMoves[i] |= position >> 9 & Utils::BitMaskB;
        }
    }
}
