#include "db_info.h"

namespace bitcask {

FileHandler* SeriesFileInfo::GetFileHandler(size_t addedSize) {
    if (curHandler == nullptr) {
        if (_SetFileHander(curId) == nullptr) {
            return nullptr;
        }
    }
    if (curHandler->Tell() + addedSize <= maxSize) {
        return curHandler.get();
    } else {
        size_t nxtId = (curId + 1) % MAX_FILE_ID_NUM;
        if (!states[nxtId]) {
            // TBD: Successful Update Meta File In Disk Or nullptr
            auto metaInfo = Singleton<MetaFileInfo>::Get();
            switch (name[0]) {
                case 'h':
                    metaInfo->Append({InfoLogOp::NEW_HEAD_ID, nxtId});
                    break;
                case 'b':
                    metaInfo->Append({InfoLogOp::NEW_BODY_ID, nxtId});
                    break;
                default: break;
            }

            UpdateFileId(nxtId);
            return _SetFileHander(nxtId);
        } else {
            return nullptr;
        }
    }
}

FileHandler* SeriesFileInfo::_SetFileHander(int id) {
    auto    cxt    = Singleton<Context>::Get();
    FileOp* fileOp = cxt->GetFileOp();
    curHandler =
        std::move(fileOp->Open(FilePathHelper::HeadFilePath(id), false, true));
    return curHandler.get();
}

bool MetaFileInfo::Load() {
    if (loaded) {
        return false;
    }
    if (handler == nullptr) {
        auto    cxt    = Singleton<Context>::Get();
        FileOp* fileOp = cxt->GetFileOp();
        handler        = std::move(
            fileOp->Open(FilePathHelper::MetaFilePath(), true, false));
    }

    if (handler == nullptr) {
        return false;
    }
    // TBD: Load setting
    size_t infoSize = sizeof(InfoLog);
    Chars  buf(infoSize);
    auto   normalFiles = Singleton<SeriesFileInfos>::Get();
    while (handler->Read(&buf) == infoSize) {
        InfoLog* log = reinterpret_cast<InfoLog*>(&buf.chrs[0]);
        switch (log->op) {
            case InfoLogOp::HEAD_MAX_FILE_SIZE:
                normalFiles->headInfo.maxSize = log->value;
                break;
            case InfoLogOp::BODY_MAX_FILE_SIZE:
                normalFiles->bodyInfo.maxSize = log->value;
                break;
            case InfoLogOp::NEW_BODY_ID:
                normalFiles->bodyInfo.UpdateFileId(log->value);
                break;
            case InfoLogOp::NEW_HEAD_ID:
                normalFiles->headInfo.UpdateFileId(log->value);
                break;

            default: break;
        }
    }
    handler.reset(nullptr);  // for append
    return true;
}
bool MetaFileInfo::Append(const InfoLog& log) {
    if (!loaded) {
        return false;
    }
    if (handler == nullptr) {
        auto    cxt    = Singleton<Context>::Get();
        FileOp* fileOp = cxt->GetFileOp();
        handler        = std::move(
            fileOp->Open(FilePathHelper::MetaFilePath(), false, true));
    }
    if (handler == nullptr) {
        return false;
    } else {
        Chars buf((char*)&log, sizeof(log));
        handler->Write(buf);
        return true;
    }
}
}  // namespace bitcask