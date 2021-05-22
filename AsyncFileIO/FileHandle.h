#pragma once

#include<future>

namespace spdlog {
    class logger;
}

namespace AsyncFileIO
{

class FileMode;
struct IOStatus;
class IOBuffer;

class FileHandle {

public:

    FileHandle(spdlog::logger& log, const std::string &path, const FileMode& mode);

    virtual  ~FileHandle();

    void setFileSize(size_t size);

    std::future<IOStatus>  write(size_t offset, const IOBuffer& buffer) const;
    std::future<IOStatus>  read(size_t  offset, IOBuffer& buffer) const;

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