#pragma once

#include <cstddef>
#include <cstdint>


namespace libcpp {

// Writes a character to stdout
int putchar(char c);

// Writes an object to stdout.
// The objects are written as if by reinterpreting the buffer as an array of uchar and calling putc size * count times
int write(void* buffer, size_t size, size_t count);

// Writes every character from the null-terminated string str and one additional newline character '\n' to the output
// stream stdout, as if by repeatedly executing putc. 
int puts(const char* str);

// Same as puts
int println(const char* str);

// Same as puts, but without the ending '\n'
int print(const char* str);


int printf(const char* format, ...);

}
