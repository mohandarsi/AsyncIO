
#include<iostream>
#include <Windows.h>


#include "FileHandle.h"
#include "FileMode.h"
#include "Overlapped.h"
#include "Utils.h"

namespace FileAPI
{

/*Open file.
"r"	read: Open file for input operations. The file must exist.
"r+"	read/update: Open a file for update (both for input and output). The file must exist.
"w"	write: Create an empty file for output operations.
If a file with the same name already exists,its contents are discarded and the file is treated as a new empty file.
w+"	write/update: Create an empty file and open it for update (both for input and output).
If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.

*/
FileHandle::FileHandle(
    const std::string &path, const FileMode& mode)
{
    DWORD access = 0, creation = 0, share_mode = 0;

    if (mode.read) {
        access = GENERIC_READ;
        share_mode = FILE_SHARE_READ;
        creation = OPEN_EXISTING;
        if (mode.extended) {
            access |= GENERIC_WRITE;
        } else {
            share_mode |= FILE_SHARE_WRITE;
        }
    } else if (mode.write) {
        access = GENERIC_WRITE;
        share_mode = FILE_SHARE_READ;
        creation = CREATE_ALWAYS;
        if (mode.extended) {
            access |= GENERIC_READ;
        }
   
    }
	else if (mode.append) {
		access = FILE_APPEND_DATA;
		share_mode = FILE_SHARE_READ;
		creation = OPEN_ALWAYS;
		if (mode.extended) {
			access |= GENERIC_READ;
		}

	}
	else {
        throw std::exception("Not supported format");
    }

    const auto hFileHandle = CreateFileA(path.c_str(), access, share_mode,
                NULL, creation, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                0);
   
    if (hFileHandle == INVALID_HANDLE_VALUE) {
        auto error = ::GetLastError();
		std::cout << "Error in CreateFileA :" << getErrorDescription(error) << "Path : "<< path << "\n";
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
    m_hFileHandle = std::shared_ptr<void>(hFileHandle, CloseHandle);

    m_bclosed = false;
}

FileHandle::~FileHandle()
{
    if (!m_bclosed) {
        Close();
    }
	std::cout << " \n DEBUG:Destructor FileHandle \n";
}

void FileHandle::SetFileSize(size_t size)
{
	//auto result = SetFilePointer(this->m_hFileHandle, size, NULL, FILE_BEGIN);
	LARGE_INTEGER filePosition = { 0 };
	filePosition.QuadPart = size;
	auto result = SetFilePointerEx(m_hFileHandle.get(), filePosition,NULL, FILE_BEGIN);
	if (result == INVALID_SET_FILE_POINTER)
	{
		std::cout << "ERROR: SetFilePointer failed: " << getLastSystemError();
	}
	else
	{
		if (!SetEndOfFile(m_hFileHandle.get()))
		{
			std::cout << "ERROR: SetEndOfFile failed : " << getLastSystemError();
		}
	}
}

void
FileHandle::Close()
{
	CancelIoEx(m_hFileHandle.get(),NULL);
	m_hFileHandle.reset(INVALID_HANDLE_VALUE);
    
	m_bclosed = true;
	std::cout << "FileHandle Closed \n";
    /* Current requests are aborted by caller. */
}


std::future<IOStatus>
FileHandle::Write(const Offset fileOffset, const void* buffer, const size_t len)
{
    
    std::cout<<"DEBUG::FileHandle::Write enter \n";
    auto overlapped = std::make_shared<std::unique_ptr<Overlapped>>(new Overlapped());
    (*overlapped)->handle = m_hFileHandle;

    std::promise<IOStatus> &statusPromise = overlapped->get()->status;

    if (buffer == nullptr || len <= 0) {
        statusPromise.set_value({ Status::OTHER_FAILURE,0 });
        return statusPromise.get_future();
    }

    if (fileOffset != OFFSET_NONE) {
        // Write the data. 
        DWORD offset = (DWORD)fileOffset;
        (*overlapped)->Offset = offset & 0xFFFFFFFF;
        (*overlapped)->OffsetHigh = (DWORD)(offset >> sizeof(offset) * 8);
    }

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(m_hFileHandle.get(),buffer, len, &numberOfBytesWritten,overlapped->get());

    auto error = ::GetLastError();
   
    switch (error)
    {
    case ERROR_SUCCESS:
    case ERROR_IO_PENDING:
        overlapped->release();
        break;
    default: 
        statusPromise.set_value({ MapError(error),numberOfBytesWritten });
        if (auto handle = (*overlapped)->handle.lock())
        {
            std::cout << "WriteFileEx" << handle << ": Error " << getErrorDescription(error);
        }
           
    }

    std::cout << "DEBUG:FileHandle::Write exit \n";
    return statusPromise.get_future();
   
}

}