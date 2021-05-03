#pragma once
#include "cache.h"
#include "chars.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace bitcask {

class QuickCache : public Cache {
  public:
    typedef std::unordered_map<std::string, uint64_t> PM;

    QuickCache(size_t size)
        : m_size(size), m_values(size) {
        m_keyPosMap    = std::make_unique<PM>();
        m_newKeyPosMap = std::make_unique<PM>();
    }
    bool Read(const Chars& key, Chars* o_value) {
        auto iter = m_keyPosMap->find(key.Str());
        if (iter == m_keyPosMap->end()) {
            return false;
        }
        uint64_t pos = iter->second;
        if (pos + m_size <= m_pos) {
            return false;
        }
        *o_value = m_values[pos % m_size];
        return true;
    }
    bool Write(const Chars& key, const Chars& value) {
        uint64_t pos = m_pos++;
        m_keyPosMap->emplace(key.Str(), pos);
        if (m_keyPosMap->size() > 10 * m_size) {
            m_newKeyPosMap->emplace(key.Str(), pos);
            if (m_newKeyPosMap->size() >= m_size) {
                m_keyPosMap    = std::move(m_newKeyPosMap);
                m_newKeyPosMap = std::make_unique<PM>();
            }
        }
        m_values[pos % m_size] = value;
        return true;
    }

  private:
    size_t              m_size = 0;
    uint64_t            m_pos  = 0;
    std::vector<Chars>  m_values;
    std::unique_ptr<PM> m_keyPosMap;
    std::unique_ptr<PM> m_newKeyPosMap;
};
}  // namespace bitcask