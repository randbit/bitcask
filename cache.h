#pragma once
#include "chars.h"
#include <memory>

namespace bitcask {

class Cache {
  public:
    virtual bool Read(const Chars& key, Chars* o_value)      = 0;
    virtual bool Write(const Chars& key, const Chars& value) = 0;
};
}  // namespace bitcask