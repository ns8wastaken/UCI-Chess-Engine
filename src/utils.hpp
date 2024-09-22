#pragma once
#include <cstdint>
#include <string>

#include "pieces.hpp"


typedef uint64_t Bitboard;
typedef uint8_t Square;


#define s_cast(T, x) static_cast<T>(x)


namespace Utils
{

    [[nodiscard]] inline uint64_t BitShift(uint64_t x, int shift)
    {
        return ((shift > 0) ? (x << shift) : (x >> -shift));
    }


    [[nodiscard]] inline constexpr bool isPieceWhite(int piece)
    {
        return !(piece & 1); // !(piece % 2)
    }


    [[nodiscard]] inline int BitCounter(uint64_t x)
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


    [[nodiscard]] std::string toUCI(const Square& square)
    {
        return std::string{
            static_cast<char>('a' + (square % 8)),
            static_cast<char>('1' + (square / 8))
        };
    }


    [[nodiscard]] std::string toUCI(const Pieces::Move& move)
    {
        std::string uci{
            static_cast<char>('a' + (move.fromSquare % 8)),
            static_cast<char>('1' + (move.fromSquare / 8)),
            static_cast<char>('a' + (move.toSquare % 8)),
            static_cast<char>('1' + (move.toSquare / 8))
        };

        if (move.promotion != Pieces::Piece::NONE) {
            uci += Pieces::getPieceChar(move.promotion);
        }

        return uci;
    }


    [[nodiscard]] Square squareFromUCI(const std::string& UCI_Square)
    {
        return static_cast<uint8_t>((UCI_Square[1] - '1') * 8 + UCI_Square[0] - 'a');
    }


    [[nodiscard]] Pieces::Move moveFromUCI(const std::string& UCI_Move)
    {
        uint8_t promotion = Pieces::Piece::NONE;

        if (UCI_Move.length() == 5) {
            promotion = Pieces::getPieceFromChar(UCI_Move[4]);
        }

        return Pieces::Move{
            static_cast<uint8_t>((UCI_Move[1] - '1') * 8 + UCI_Move[0] - 'a'),
            static_cast<uint8_t>((UCI_Move[3] - '1') * 8 + UCI_Move[2] - 'a'),
            promotion
        };
    }


    /*
        Board flags

        Castling:

            Black kingside
                        |   ┌- White kingside
              0 0 0 0 1 1 1 1
                      |   └- White queenside
            Black queenside
    */

    [[nodiscard]] inline bool canCastleKingside(const uint16_t& flags, const bool& isWhite)
    {
        return flags & (isWhite ? 1 : 4);
    }

    [[nodiscard]] inline bool canCastleQueenside(const uint16_t& flags, const bool& isWhite)
    {
        return flags & (isWhite ? 2 : 8);
    }


    /*
                      Bitmasks

            BitMaskA           BitMaskB
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0
        0 1 1 1 1 1 1 1     1 1 1 1 1 1 1 0

            BitMaskA2          BitMaskB2
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
        0 0 1 1 1 1 1 1     1 1 1 1 1 1 0 0
    */

    constexpr uint64_t BitMaskA  = ~0x101010101010101ULL;
    constexpr uint64_t BitMaskA2 = ~0x303030303030303ULL;

    constexpr uint64_t BitMaskB  = ~0x8080808080808080ULL;
    constexpr uint64_t BitMaskB2 = ~0xc0c0c0c0c0c0c0c0ULL;


    constexpr uint64_t B_PawnStart = 0xff000000000000;
    constexpr uint64_t W_PawnStart = 0xff00;

}
