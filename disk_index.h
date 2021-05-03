#pragma once
#include "chars.h"
#include <memory>

namespace bitcask {

struct DiskIndex {
    Chars key;
    int   version;
    int   valueFileId;
    int   valueOffset;
    int   valueLength;

    DiskIndex() {}
    DiskIndex(const Chars& key) : key(key) {}

    size_t Size() {
        return 20 + key.len;
    }
};
}  // namespace bitcask