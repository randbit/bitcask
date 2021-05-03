#pragma once
#include <iostream>
#include <memory>
#include <string>

namespace bitcask {

struct Chars {
    std::unique_ptr<char[]> chrs;
    size_t                  len;

    Chars(size_t len = 0) : len(len) {
        if (len > 0) {
            chrs.reset(new char[len]);
        }
    }

    Chars(const Chars& other) {
        *this = other;
    }

    Chars(const char* s, size_t len) {
        Set(s, len);
    }

    Chars& operator=(const Chars& other) {
        Set(&other.chrs[0], other.len);
        return *this;
    }

    void Set(const char* s, size_t len) {
        this->len = len;
        chrs.reset(new char[len]);
        std::copy(s, s + len, &chrs[0]);
    }

    std::string Str() const {
        return std::string(&chrs[0], len);
    }

    void Print() const {
        std::cout << Str() << std::endl;
    }
};
}  // namespace bitcask