#include "cache.h"
#include "chars.h"
#include "disk_mgr.h"
#include "hash_table.h"
#include "db_info.h"
#include <cstdio>
#include <vector>

namespace bitcask {

class BitcaskDB {
  public:
    BitcaskDB() {
    }

    int Read(const Chars& key, Chars* o_value) {
        DiskIndex index;
        return m_cache->Read(key, o_value) ||
               (m_hashTable->Read(key, &index) &&
                m_diskMgr->Read(index, o_value) &&
                m_cache->Write(key, *o_value));
    }

    bool Write(const Chars& key, const Chars& value) {
        DiskIndex index(key);
        return m_diskMgr->Write(value, &index) && m_hashTable->Write(index);
    }

  private:
    std::unique_ptr<Cache>     m_cache;
    std::unique_ptr<HashTable> m_hashTable;
    std::unique_ptr<DiskMgr>   m_diskMgr;
};
}  // namespace bitcask

int main() {
    return 0;
}