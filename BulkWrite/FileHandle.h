#pragma once

#include<future>
#include "Definitions.h"

namespace spdlog {
    class logger;
}

namespace FileAPI
{

class FileMode;

class FileHandle {

public:

    FileHandle(spdlog::logger& log, const std::string &path, const FileMode& mode);

    virtual  ~FileHandle();

    virtual void setFileSize(size_t size);

    std::future<IOStatus>  write(Offset offset, const void* buffer, size_t numberOfBytesToWrite) const;
    std::future<IOStatus>  read(Offset offset, void* buffer, size_t numberOfBytesToRead) const;
    
    bool isClosed() const { return m_closed;}

private:

    void close();
    HANDLE createFile(const std::string &path, const FileMode& mode) const;

private:
    spdlog::logger& m_logger;

    friend class OverlappedIOController;

    std::shared_ptr<void> m_fileHandle;

    bool m_closed;

};

}