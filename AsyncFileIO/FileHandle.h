#pragma once

#include<future>

namespace spdlog {
    class logger;
}

namespace AsyncFileIO
{

class FileMode;
struct IOStatus;

class FileHandle {

public:

    FileHandle(spdlog::logger& log, const std::string &path, const FileMode& mode);

    virtual  ~FileHandle();

    void setFileSize(size_t size);

    std::future<IOStatus>  write(int64_t  offset, const void* buffer, size_t numberOfBytesToWrite) const;
    std::future<IOStatus>  read(int64_t  offset, void* buffer, size_t numberOfBytesToRead) const;

private:

    void close();
    bool isClosed() const { return m_closed; }
    HANDLE createFile(const std::string &path, const FileMode& mode) const;

private:
    spdlog::logger& m_logger;

    friend class OverlappedIOController;

    std::shared_ptr<void> m_fileHandle;

    bool m_closed;

};

}