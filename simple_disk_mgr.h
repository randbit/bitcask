#pragma once
#include "chars.h"
#include "db_info.h"
#include "disk_index.h"
#include "file_op.h"
#include "hash_table.h"
#include <memory>

namespace bitcask {

class DiskMgr {
  public:
    bool Read(const DiskIndex& index, Chars* o_value) {
        const std::string& path =
            FilePathHelper::BodyFilePath(index.valueFileId);
        auto fileOp  = Singleton<Context>::Get()->GetFileOp();
        auto handler = fileOp->Open(path, true, false);
        handler->Seek(index.valueOffset);
        o_value->len = index.valueLength;
        return handler->Read(o_value) == index.valueLength;
    }
    bool Write(const Chars& value, DiskIndex* o_index) {
        auto infos           = Singleton<SeriesFileInfos>::Get();
        auto headHdlr        = infos->headInfo.GetFileHandler(o_index->Size());
        auto bodyHdlr        = infos->bodyInfo.GetFileHandler(value.len);
        o_index->valueLength = value.len;
        o_index->valueOffset = bodyHdlr->Tell();
        o_index->valueFileId = infos->bodyInfo.curId;
        bodyHdlr->Write(value);
        size_t intSize = sizeof(size_t);
        headHdlr->Write(
            Chars(reinterpret_cast<char*>(&o_index->key.len), intSize));
        headHdlr->Write(Chars(&(o_index->key.chrs[0]), o_index->key.len));
        headHdlr->Write(
            Chars(reinterpret_cast<char*>(&o_index->valueFileId), intSize));
        headHdlr->Write(
            Chars(reinterpret_cast<char*>(&o_index->valueLength), intSize));
        headHdlr->Write(
            Chars(reinterpret_cast<char*>(&o_index->valueOffset), intSize));
        return true;
    }
    bool Load(HashTable* hashTable) {
        auto infos     = Singleton<SeriesFileInfos>::Get();
        auto headFiles = infos->headInfo.GetExistedFiles();
        for (int id : headFiles) {
            const auto& path    = FilePathHelper::HeadFilePath(id);
            auto        fileOp  = Singleton<Context>::Get()->GetFileOp();
            auto        handler = fileOp->Open(path, true);
            DiskIndex   index;
            while (LoadNext(handler.get(), &index)) {
                hashTable->Write(index);
            }
        }
        return true;
    }

  private:
    bool LoadNext(FileHandler* handler, DiskIndex* o_index) {
        auto charsToInt = [](const Chars& c) {
            return *(reinterpret_cast<size_t*>(&c.chrs[0]));
        };
        size_t intSize = sizeof(size_t);
        Chars  i(intSize);
        handler->Read(&i);
        size_t len = charsToInt(i);
        Chars  keyChars(len);
        handler->Read(&keyChars);
        o_index->key = keyChars;
        handler->Read(&i);
        o_index->valueFileId = charsToInt(i);
        handler->Read(&i);
        o_index->valueLength = charsToInt(i);
        handler->Read(&i);
        o_index->valueOffset = charsToInt(i);
        // TBD: while false
        return true;
    }
};
}  // namespace bitcask