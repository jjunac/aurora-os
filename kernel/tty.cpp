#include "kernel/tty.hpp"

#include <algorithm>


namespace kernel {

uint16_t* const Tty::kVgaBufferBegin = reinterpret_cast<uint16_t*>(0xB8000);
uint16_t* const Tty::kVgaBufferEnd = kVgaBufferBegin + kVgaBufferSize;

void Tty::init() {
    curVgaBuffer_ = kVgaBufferBegin;
    clear();
}

void Tty::putchar(uchar c) {
    if (curVgaBuffer_ == kVgaBufferEnd) {
        scroll();
    }

    switch(c) {
    case '\n':
        newLine();
        break;
    default:
        *curVgaBuffer_ = constructVgaEntry(c);
        curVgaBuffer_++;
    }
}

void Tty::clear() {
    fill(0);
}

void Tty::fill(uchar c) {
    std::fill(kVgaBufferBegin, kVgaBufferEnd, constructVgaEntry(c));
}

uint16_t Tty::constructVgaEntry(uchar c) {
    return static_cast<uint16_t>(VgaColor::kBlack) << 12
            | static_cast<uint16_t>(VgaColor::kWhite) << 8
            | static_cast<uint16_t>(c);
}

void Tty::newLine() {
    const size_t writtenOnLine = (curVgaBuffer_ - kVgaBufferBegin) % kVgaWidth;
    curVgaBuffer_ += kVgaWidth - writtenOnLine;
}

void Tty::scroll() {
    // Shift everything left of 1 line
    uint16_t* src = kVgaBufferBegin + kVgaWidth;
    uint16_t* dst = kVgaBufferBegin;
    for (; src != kVgaBufferEnd; ++src, ++dst) {
        *dst = *src;
    }
    // Then fill the "new last line" with NULL
    for (; dst != kVgaBufferEnd; ++dst) {
        *dst = constructVgaEntry(0);
    }
    // And finally shift left the current position of 1 line
    curVgaBuffer_ -= kVgaWidth;
}



}