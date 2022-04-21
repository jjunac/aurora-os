#include "libcpp/stdio.hpp"

#include "kernel/tty.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <type_traits>

#include "kernel/kernel.hpp"

namespace libcpp {

namespace __itoa {
    static constexpr char digits[200] = {
        '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
        '1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
        '2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
        '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
        '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
        '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
        '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
        '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
        '8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
        '9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
    };

    template <typename T>
    inline char* append1(char* buf, T i) {
        buf[0] = '0' + static_cast<char>(i);
        return buf + 1;
    }

    template <typename T>
    inline char* append2(char* buf, T i) {
        const size_t pos = 2*i;
        buf[0] = digits[pos];
        buf[1] = digits[pos+1];
        return buf + 2;
    }
}

template <typename T, std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, bool> = true>
char* itoa_10(T value, char* str) {
    while (value >= 100) {
        const T rem = value % 100;
        value /= 100;
        str = __itoa::append2(str, rem);
    }
    if (value >= 10) {
        str = __itoa::append2(str, value);
    } else {
        str = __itoa::append1(str, value);
    }
    return str;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, bool> = true>
char* itoa_10(T value, char* str) {
    if (value < 0) {
        *str = '-';
        ++str;
        value = -value;
    }
    return itoa_10(static_cast<typename std::make_unsigned<T>::type>(value), str);
}

int putchar(char c) {
    kernel::tty().Putchar(c);
    return 0;
}

int write(void* buffer, size_t size, size_t count) {
    uchar* buf = reinterpret_cast<uchar*>(buffer);
    const uchar* end = buf + (size * count);
    for (; buf < end; ++buf) {
        kernel::tty().Putchar(*buf);
    }
    return 0;
}

int puts(const char* str) {
    return println(str);
}

int println(const char* str) {
    print(str);
    putchar('\n');
    return 0;
}

int print(const char* str) {
    for (; *str; ++str) {
        putchar(*str);
    }
    return 0;
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (; *format; ++format) {
        if (*format == '%') {
            ++format;
            switch(*format) {
            case 'd': {
                int i = va_arg(args, int);
                // Biggest int is -2147483646
                char buf[11];
                const char* end = itoa_10(i, buf);
                write(buf, sizeof(buf[0]), end - buf);
                break;
            }
            case 's': {
                const char* str = va_arg(args, const char*);
                print(str);
            }
            }
        } else {
            putchar(*format);
        }
    }
    va_end(args);
    return 0;
}

}
