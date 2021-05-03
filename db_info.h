#pragma once
#include "chars.h"
#include "context.h"
#include "file_op.h"
#include "singleton.hpp"
#include <bitset>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

namespace bitcask {

#define MAX_FILE_ID_NUM 100
#define FILE_ID_WIDTH 4

enum InfoLogOp {
    HEAD_MAX_FILE_SIZE = 1,
    BODY_MAX_FILE_SIZE = 2,
    NEW_HEAD_ID        = 3,
    NEW_BODY_ID        = 4,
};

struct InfoLog {
    InfoLogOp op;
    size_t    value;
};

struct FilePathHelper {
    static std::string HeadFilePath(int id) {
        auto               cxt = Singleton<Context>::Get();
        std::ostringstream ss;
        ss << std::setw(FILE_ID_WIDTH) << std::setfill('0') << id;
        ss << ".head";
        return Concat(cxt->GetPath(), ss.str());
    }

    static std::string BodyFilePath(int id) {
        auto               cxt = Singleton<Context>::Get();
        std::ostringstream ss;
        ss << std::setw(FILE_ID_WIDTH) << std::setfill('0') << id;
        ss << ".body";
        return Concat(cxt->GetPath(), ss.str());
    }

    static std::string MetaFilePath() {
        auto               cxt = Singleton<Context>::Get();
        std::ostringstream ss;
        ss << "meta";
        return Concat(cxt->GetPath(), ss.str());
    }

    static std::string Concat(const std::string& a, const std::string& b) {
        if (a.size() > 0 && a[a.size() - 1] == '/') {
            return a + b;
        } else {
            return a + "/" + b;
        }
    }
};

struct SeriesFileInfo {
    std::string                  name;
    std::bitset<MAX_FILE_ID_NUM> states;
    size_t                       curId;
    size_t                       maxSize;
    std::unique_ptr<FileHandler> curHandler;

    SeriesFileInfo(std::string name) : name(name) {}

    void UpdateFileId(int fileId) {
        curId = fileId;
        states.set(fileId);
    }

    std::vector<size_t> GetExistedFiles () {
        std::vector<size_t> files;
        for (int i = 0; i < MAX_FILE_ID_NUM; ++i) {
            int id = (curId + i) % MAX_FILE_ID_NUM;
            if (states[i]) {
                files.emplace_back(id);
            }
        }
        return files;
    }

    FileHandler* GetFileHandler(size_t addedSize);
    FileHandler* _SetFileHander(int id);
};

struct SeriesFileInfos {
    SeriesFileInfos() : headInfo("head"), bodyInfo("body") {}
    SeriesFileInfo headInfo;
    SeriesFileInfo bodyInfo;
};

struct MetaFileInfo {
    std::unique_ptr<FileHandler> handler;
    bool                         loaded = false;

    bool Load();
    bool Append(const InfoLog& log);
};
}  // namespace bitcask