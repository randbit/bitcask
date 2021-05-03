#pragma once
#include "chars.h"
#include "disk_index.h"
#include "file_op.h"
#include "hash_table.h"
#include <memory>

namespace bitcask {

class DiskMgr {
  public:
    virtual bool Read(const DiskIndex& index, Chars* o_value)   = 0;
    virtual bool Write(const Chars& value, DiskIndex* o_index) = 0;
    virtual bool Load(HashTable* hashTable)                  = 0;
};
}  // namespace bitcask