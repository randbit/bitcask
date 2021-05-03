#pragma once
#include "file_op.h"
#include <memory>

namespace bitcask {

class CFileHandler : public FileHandler {
  public:
    CFileHandler(FILE* file) : m_file(file) {}
    ~CFileHandler() {
        fclose(m_file);
    }
    void Seek(size_t pos) {
        fseek(m_file, pos, SEEK_SET);
    }
    size_t Tell() {
        return ftell(m_file);
    }
    size_t Read(Chars* o_buf) {
        char*  buf = &o_buf->chrs[0];
        size_t sz  = o_buf->len;
        size_t sn  = 0;
        size_t n;
        while (sn < sz && (n = fread(buf, 1, sz - sn, m_file)) > 0)
            sn += n;
        return sn;
    }
    size_t Write(const Chars& buf) {
        const char* src = &buf.chrs[0];
        size_t      sz  = buf.len;
        size_t      sn  = 0;
        size_t      n;
        while (sn < sz && (n = fwrite(src + sn, 1, sz - sn, m_file)) > 0)
            sn += n;
        return sn;
    }

  private:
    FILE* m_file;
};

class CFileOp : public FileOp {
  public:
    std::unique_ptr<FileHandler>
    Open(const std::string& path, bool readOrWrite, bool append) {
        FILE* fp =
            fopen(path.c_str(), readOrWrite ? "r" : (append ? "a" : "w"));
        return fp == nullptr ? nullptr : std::make_unique<CFileHandler>(fp);
    }
};
}  // namespace bitcask