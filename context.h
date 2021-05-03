#pragma once
#include "chars.h"
#include "disk_index.h"
#include "file_op.h"
#include "hash_table.h"
#include <memory>

namespace bitcask {

class Context {
  public:
    enum FilePlatform { C, Linux };

    const std::string& GetPath() {
        return m_path;
    }

    void SetPath(const std::string& path) {
        m_path = path;
    }

    FileOp* GetFileOp() {
        return m_fileOp.get();
    }

    void SetFileOp(std::unique_ptr<FileOp> fileOp) {
        m_fileOp = std::move(fileOp);
    }

  private:
    std::string             m_path;
    std::unique_ptr<FileOp> m_fileOp;
};
}  // namespace bitcask