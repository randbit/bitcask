#pragma once
#include "chars.h"
#include "hash_table.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>

namespace bitcask {

class SimpleHashTable : public HashTable {
  public:
    SimpleHashTable(size_t size) : m_size(size), m_mutexes(size) {}
    bool Read(const Chars& key, DiskIndex* o_index) {
        const std::string& s = key.Str();
        size_t which = BKDRHash(s) % m_size;
        std::shared_lock<std::mutex> lock(m_mutexes[which]);
        auto iter = m_data[which].find(s);
        if (iter == m_data[which].end()) {
            return false;
        }
        *o_index = iter->second;
        return true;
    }
    bool Write(const DiskIndex& index) {
        const std::string& s = index.key.Str();
        size_t which = BKDRHash(s) % m_size;
        std::unique_lock<std::mutex> lock(m_mutexes[which]);
        m_data[which].emplace(s, index);
        return true;
    }

  private:
    uint32_t BKDRHash(const std::string& str) {
        uint32_t seed = 131;  // 31 131 1313 13131 131313 etc..
        uint32_t hash = 0;
        for (int i = 0; i < str.size(); ++i) {
            hash = hash * seed + str[i];
        }
        return (hash & 0x7FFFFFFF);
    }

    size_t                                                  m_size;
    std::vector<std::unordered_map<std::string, DiskIndex>> m_data;
    std::vector<std::mutex>                                 m_mutexes;
};
}  // namespace bitcask