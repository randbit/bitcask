#pragma once
#include "chars.h"
#include "disk_index.h"
#include <memory>

namespace bitcask {

class HashTable {
  public:
    virtual bool Read(const Chars& key, DiskIndex* o_index) = 0;
    virtual bool Write(const DiskIndex& index)              = 0;
};
}  // namespace bitcask