#pragma once
#include "chars.h"
#include "disk_index.h"
#include "file_op.h"
#include "hash_table.h"
#include <memory>

namespace bitcask {

template <typename T>
struct Singleton {
  static T* Get() {
      if (m_instance == nullptr) {
          m_instance = std::make_unique<T>();
      }
      return m_instance.get();
  }
  static std::unique_ptr<T> m_instance;
};
}  // namespace bitcask