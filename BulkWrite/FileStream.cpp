
#include "spdlog/spdlog.h"

#include "FileStream.h"
#include "FileHandle.h"
#include "FileMode.h"

namespace FileAPI
{

FileStream::FileStream(spdlog::logger& log, const std::string &path, const FileMode& mode)
    : m_logger(log)
    , m_state(FileState::CLOSED)
    , m_currentPos(0)
    , m_ptrFileHandle(std::make_unique<FileHandle>(log,path,mode))
{
    m_logger.trace("FileStream");
    setName(path);
    m_state = FileState::OPENED;
}

FileStream::~FileStream()
{
    m_logger.trace("~FileStream");
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

FileAPI::Offset FileStream::seek(const Offset position, const RelativePosition relativePosition)
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
FileStream::write(const void *data, const size_t length, Offset offset)
{
    m_logger.debug("write with size {} and offset {}", length, offset);

    if (offset == OFFSET_NONE)
    {
        offset = m_currentPos;
    }
    return m_ptrFileHandle->write(offset, data, length);
}

std::future<IOStatus>
FileStream::read(void *data, size_t length, Offset offset)
{
    m_logger.debug("read with size {} and offset {}", length, offset);
    if (offset == OFFSET_NONE)
    {
        offset = m_currentPos;
    }
    return m_ptrFileHandle->read(offset, data, length);
}

void FileStream::writeSync(const void *data, const size_t length, const Offset offset)
{
    m_logger.trace("writeSync");
    write(data, length, offset).wait();
}

}