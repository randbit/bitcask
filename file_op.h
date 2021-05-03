#pragma once
#include "chars.h"
#include <memory>

namespace bitcask {

class FileHandler {
  public:
    virtual void   Seek(size_t pos)        = 0;
    virtual size_t Tell()                  = 0;
    virtual size_t Read(Chars* o_buf)      = 0;
    virtual size_t Write(const Chars& buf) = 0;
};

class FileOp {
  public:
    virtual std::unique_ptr<FileHandler>
    Open(const std::string& path, bool readOrWrite, bool append = false) = 0;
};
}  // namespace bitcask