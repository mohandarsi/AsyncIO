
#include "logging.h"
#include "Definitions.h"
#include "FileHandle.h"
#include "FileMode.h"
#include "Overlapped.h"
#include "Utils.h"
#include "IOBuffer.h"

namespace AsyncFileIO
{

/*Open file.
"r"	read: Open file for input operations. The file must exist.
"r+"	read/update: Open a file for update (both for input and output). The file must exist.
"w"	write: Create an empty file for output operations.
If a file with the same name already exists,its contents are discarded and the file is treated as a new empty file.
w+"	write/update: Create an empty file and open it for update (both for input and output).
If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.

*/
FileHandle::FileHandle(spdlog::logger& log, const std::string &path, const FileMode& mode)
    : m_logger(log)
    , m_closed(false)
{
    SPDLOG_LOGGER_TRACE(m_logger, "FileHandle::FileHandle");

    m_fileHandle = std::shared_ptr<void>(createFile(path, mode), CloseHandle);

    m_logger.debug("FileHandle {0} opened for path {1}", m_fileHandle.get(), path);
}

FileHandle::~FileHandle()
{
    SPDLOG_LOGGER_TRACE(m_logger, "FileHandle::~FileHandle");

    if (!isClosed()) {
        close();
    }
}

void FileHandle::setFileSize(size_t size)
{
    m_logger.debug("FileHandle::setFileSize handle {0} size {1}", m_fileHandle.get(), size);

    LARGE_INTEGER filePosition;
    filePosition.QuadPart = size;

    const auto result = SetFilePointerEx(m_fileHandle.get(), 
        filePosition, nullptr, FILE_BEGIN);

    if (result == INVALID_SET_FILE_POINTER)
    {
        m_logger.error("FileHandle::SetFilePointer failed with error:{}", getLastSystemError());
        return;
    }

    if (!SetEndOfFile(m_fileHandle.get()))
    {
        m_logger.error("FileHandle::SetEndOfFile failed with error:{}", getLastSystemError());
    }
}

void
FileHandle::close()
{
    m_logger.debug("FileHandle::close closing file handle {0}", m_fileHandle.get());
    CancelIoEx(m_fileHandle.get(), nullptr);
    m_fileHandle.reset();
    m_closed = true;
}

std::future<IOStatus>
FileHandle::write(const size_t offset, const IOBuffer& buffer) const
{
    m_logger.debug("Write {0} enter with buffer with size {1} at file offset {2}", m_fileHandle.get(), buffer.size(), offset);

    auto overlapped = std::make_shared<std::unique_ptr<Overlapped>>(new Overlapped(m_fileHandle, offset));

    auto& statusPromise = overlapped->get()->status;
    auto statusFuture = statusPromise.get_future();

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(m_fileHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &numberOfBytesWritten, overlapped->get());

    const auto error = ::GetLastError();

    switch (error)
    {
    case ERROR_SUCCESS:
    case ERROR_IO_PENDING:
        // we are just releasing the pointer not memory. Actual memory gets deallocated in callback of overlapped
        overlapped->release();
        break;
    default:
        statusPromise.set_value({ MapError(error),numberOfBytesWritten });
        m_logger.error("WriteFileEx failed with error {0} for handle {1}", getErrorDescription(error), m_fileHandle.get());
    }

    return statusFuture;
}

std::future<IOStatus>
FileHandle::read(const size_t fileOffset, IOBuffer& buffer) const
{
    m_logger.debug("read {0} enter with buffer with size {1} at file offset {2}", m_fileHandle.get(), buffer.size(), fileOffset);
	
    auto overlapped = std::make_shared<std::unique_ptr<Overlapped>>(new Overlapped(m_fileHandle, fileOffset));

    auto& statusPromise = overlapped->get()->status;
    auto statusFuture = statusPromise.get_future();

    DWORD numberOfBytesRead = 0;
    const auto result = ::ReadFile(m_fileHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &numberOfBytesRead, overlapped->get());

    if (result) return statusFuture;

    const auto error = ::GetLastError();

    switch (error)
    {
    case ERROR_SUCCESS:
    case ERROR_IO_PENDING:
        overlapped->release();
        break;
    default:
        m_logger.error("ReadFile failed with error {0} for handle {1}", getErrorDescription(error), m_fileHandle.get());
        statusPromise.set_value({ MapError(error),numberOfBytesRead });
    }

    return statusFuture;
}

HANDLE FileHandle::createFile(const std::string &path, const FileMode& mode) const
{
    DWORD access, creation, shareMode;

    if (mode.read) {
        access = GENERIC_READ;
        shareMode = FILE_SHARE_READ;
        creation = OPEN_EXISTING;
        if (mode.extended) {
            access |= GENERIC_WRITE;
        }
        else {
            shareMode |= FILE_SHARE_WRITE;
        }
    }
    else if (mode.write) {
        access = GENERIC_WRITE;
        shareMode = FILE_SHARE_READ;
        creation = CREATE_ALWAYS;
        if (mode.extended) {
            access |= GENERIC_READ;
        }
    }
    else if (mode.append) {
        access = FILE_APPEND_DATA;
        shareMode = FILE_SHARE_READ;
        creation = OPEN_ALWAYS;
        if (mode.extended) {
            access |= GENERIC_READ;
        }
    }
    else {
        throw std::exception("Not supported format");
    }

    auto* const hFileHandle = CreateFileA(path.c_str(), access, shareMode,
        nullptr, creation,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        nullptr);

    if (hFileHandle == INVALID_HANDLE_VALUE) {
        const auto error = ::GetLastError();
        m_logger.error("CreateFileA failed with error:{}", getErrorDescription(error));
        switch (error) {
        case ERROR_ACCESS_DENIED:
            throw std::exception("Insufficient permissions for file opening");
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
            throw std::exception("File already exists");
        case ERROR_FILE_NOT_FOUND:
            throw std::exception("File not found");
        default:
            throw std::exception("Failed to open file");
        }
    }

    return hFileHandle;
}

}