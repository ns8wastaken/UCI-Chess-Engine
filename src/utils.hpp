#pragma once
#include <cstdint>
#include <string>

#include "pieces.hpp"


#ifndef UTIL_TYPES
#define UTIL_TYPES
typedef uint64_t Bitboard;
typedef uint8_t Square;
#endif


namespace Utils
{

    [[nodiscard]]
    inline uint64_t BitShift(uint64_t x, int shift)
    {
        return ((shift > 0) ? (x << shift) : (x >> -shift));
    }


    [[nodiscard]]
    inline constexpr bool isPieceWhite(int piece)
    {
        return !(piece & 1); // !(piece % 2)
    }


    [[nodiscard]]
    inline int BitCounter(uint64_t x)
    {
        int bitCount = 0;
        while (x) {
            bitCount += (x & 0x1);
            x >>= 1;
        }
        return bitCount;
    }


    inline void setBit(uint64_t& bitboard, Square square)
    {
        bitboard |= (1ULL << square);
    }


    inline void clearBit(uint64_t& bitboard, Square square)
    {
        bitboard &= ~(1ULL << square);
    }


    [[nodiscard]]
    std::string toUCI(const Square from, const Square to)
    {
        return std::string{
            static_cast<char>('a' + (from % 8)),
            static_cast<char>('1' + (from / 8)),
            static_cast<char>('a' + (to % 8)),
            static_cast<char>('1' + (to / 8))
        };
    }

    [[nodiscard]]
    std::string toUCI(const Pieces::Move& move)
    {
        return std::string{
            static_cast<char>('a' + (move.fromSquare % 8)),
            static_cast<char>('1' + (move.fromSquare / 8)),
            static_cast<char>('a' + (move.toSquare % 8)),
            static_cast<char>('1' + (move.toSquare / 8))
        };
    }


    [[nodiscard]]
    Pieces::Move fromUCI(const std::string& UCI_Move)
    {
        return Pieces::Move{
            static_cast<uint8_t>(((UCI_Move[1] - '1') ^ 7) * 8 + UCI_Move[0] - 'a'),
            static_cast<uint8_t>(((UCI_Move[3] - '1') ^ 7) * 8 + UCI_Move[2] - 'a')
        };
    }


    /*
        Bit bitmasks

            BitMaskA           BitMaskB
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B
        A * * * * * * *     * * * * * * * B

            BitMaskA2          BitMaskB2
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
        A A * * * * * *     * * * * * * B B
    */

    constexpr uint64_t BitMaskA = ~0x8080808080808080ULL;
    constexpr uint64_t BitMaskA2 = ~0xc0c0c0c0c0c0c0c0ULL;

    constexpr uint64_t BitMaskB = ~0x101010101010101ULL;
    constexpr uint64_t BitMaskB2 = ~0x303030303030303ULL;


    constexpr uint64_t B_PawnStart = 0xff000000000000;
    constexpr uint64_t W_PawnStart = 0xff00;

}
