#pragma once

#include "kernel/kdefs.hpp"

namespace kernel {

class Tty {
public:
    void init();

    void putchar(uchar c);
    void clear();
    void fill(uchar c);

private:
    enum VgaColor {
        kBlack = 0,
        kBlue = 1,
        kGreen = 2,
        kCyan = 3,
        kRed = 4,
        kMagenta = 5,
        kBrown = 6,
        kLightGrey = 7,
        kDarkGrey = 8,
        kLightBlue = 9,
        kLightGreen = 10,
        kLightCyan = 11,
        kLightRed = 12,
        kLightMagenta = 13,
        kLightBrown = 14,
        kWhite = 15,
    };

    static constexpr size_t     kVgaWidth           = 80;
    static constexpr size_t     kVgaHeight          = 25;
    static constexpr size_t     kVgaBufferSize      = kVgaWidth * kVgaHeight;
    // Cannot be constexpr because the standard doesn't allow int to ptr cast in constexpr, as undefined behavior
    static uint16_t* const      kVgaBufferBegin;
    static uint16_t* const      kVgaBufferEnd;

    static constexpr VgaColor   kDefaultBackground  = VgaColor::kBlack;
    static constexpr VgaColor   kDefaultForeground  = VgaColor::kWhite;

    uint16_t* curVgaBuffer_;

    uint16_t constructVgaEntry(uchar c);
    void newLine();
    void scroll();
};

}


