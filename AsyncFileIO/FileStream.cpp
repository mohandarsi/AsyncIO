
#include "spdlog/spdlog.h"

#include "FileStream.h"
#include "FileHandle.h"
#include "FileMode.h"
#include "IOBuffer.h"

namespace AsyncFileIO
{

FileStream::FileStream(spdlog::logger& log, const std::string &path, const FileMode& mode)
    : m_logger(log)
    , m_currentPos(0)
    , m_ptrFileHandle(std::make_unique<FileHandle>(log,path,mode))
{
    m_logger.trace("FileStream::FileStream");
    setName(path);
}

FileStream::~FileStream()
{
    m_logger.trace("FileStream::~FileStream");
}

std::string
FileStream::getName() const
{
    return m_szName;
}

void FileStream::setSize(const size_t length)
{
    m_ptrFileHandle->setFileSize(length);
}

void
FileStream::setName(const std::string& newName)
{
    m_szName = newName;
}

AsyncFileIO::Offset FileStream::seek(const Offset position, const RelativePosition relativePosition)
{
    auto newPosition = m_currentPos;
    if (relativePosition == RelativePosition::CURRENT) {
        newPosition += position;
    } else {
        newPosition = position;
    }
    if (newPosition < 0) {
        throw std::exception("Invalid position specified (negative)");
    }
    m_currentPos = newPosition;
    return newPosition;
}

std::future<IOStatus>
FileStream::write(const IOBuffer& buffer, Offset offset)
{
    m_logger.debug("write with size {} and offset {}", buffer.size(), offset);

    if (offset <= CURRENT_FILE_OFFSET)
    {
        offset = m_currentPos;
    }
    return m_ptrFileHandle->write(offset, buffer);
}

std::future<IOStatus>
FileStream::read(IOBuffer& buffer, Offset offset)
{
    m_logger.debug("read with size {} and offset {}", buffer.size(), offset);

    if (offset <= CURRENT_FILE_OFFSET)
    {
        offset = m_currentPos;
    }
    return m_ptrFileHandle->read(offset, buffer);
}

}