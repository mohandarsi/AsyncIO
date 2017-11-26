
#include "FileStream.h"
#include "IORequest.h"
#include "FileHandle.h"
#include "Utils.h"

FileHandle::FileHandle(HANDLE handle, HANDLE write_handle) :
		m_hFileHandle(handle), m_hFileWriteHandle(write_handle)
{

}


/*Open file.
"r"	read: Open file for input operations. The file must exist.
"r+"	read/update: Open a file for update (both for input and output). The file must exist.
"w"	write: Create an empty file for output operations.
If a file with the same name already exists,its contents are discarded and the file is treated as a new empty file.
w+"	write/update: Create an empty file and open it for update (both for input and output).
If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.

*/
FileHandle::FileHandle(
    const std::string &path, FileMode mode)
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
        assert(false);
    }

    this->m_hFileHandle = CreateFileA(path.c_str(), access, share_mode,
                NULL, creation, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                0);
   
    if (this->m_hFileHandle == INVALID_HANDLE_VALUE) {
		std::string error = GetSystemError();
		std::cout << "Error in CreateFileA :" << error.c_str() << "Path : "<< path.c_str() << "\n";
        switch (GetLastError()) {
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

    m_bclosed = false;
    m_hFileWriteHandle = INVALID_HANDLE_VALUE;
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
	auto result = SetFilePointerEx(this->m_hFileHandle, filePosition,NULL, FILE_BEGIN);
	if (result == INVALID_SET_FILE_POINTER)
	{
		std::cout << "ERROR: SetFilePointer failed (continuation): " << GetSystemError().c_str();
	}
}

void
FileHandle::SetStream(IFileStream::Ptr stream)
{
   this->m_ptrStream = stream;
}


void
FileHandle::Close()
{
	/* Any pending I/O should receive notification after that and release the
		* stream.
		*/
	std::lock(read_mutex, m_writeMutex);
	std::lock_guard<std::mutex> lk1(read_mutex, std::adopt_lock);
	std::lock_guard<std::mutex> lk2(m_writeMutex, std::adopt_lock);

	CancelIoEx(m_hFileHandle,NULL);
	DWORD error = GetLastError();
	CloseHandle(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE_VALUE;
	if (m_hFileWriteHandle != INVALID_HANDLE_VALUE) {
		CancelIoEx(m_hFileWriteHandle, NULL);
		CloseHandle(m_hFileWriteHandle);
		m_hFileWriteHandle = INVALID_HANDLE_VALUE;
	}
	m_ptrStream.reset();//= NULL
	m_bclosed = true;
	std::cout << "FileHandle Closed \n";
    /* Current requests are aborted by caller. */
}

void
FileHandle::SetWriteActivity(bool is_active)
{
    if (is_active) {
        assert(!m_bclosed);
    }
    else
    {
      m_ptrCurrentWriteRequest.reset();
    }

}

bool
FileHandle::GetWriteActivity()
{
	if (m_ptrCurrentWriteRequest != NULL && m_ptrCurrentWriteRequest.use_count() > 0) {
		return true;
	}
	else
	{
		return false;
	}
}

void
FileHandle::ReadCompleteCallback(size_t transfer_size, DWORD error)
{
	std::cout << "ERROR: ReadCompleteCallback Not Implemented";
	throw std::logic_error("Not implemented ReadCompleteCallback");
}

void
FileHandle::WriteCompleteCallback(size_t transfer_size, DWORD error)
{
    std::cout<<"DEBUG:FileHandle::WriteCompleteCallback \n";

	if (m_bclosed == true || m_hFileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "ERROR: FileHandle::WriteCompleteCallback File closed \n";
		return;
	}
	if (!GetWriteActivity())
	{
		std::cout << "No current Write activity \n";
		return;
	}
	WriteRequest* ptrCurrentWriteRequest = dynamic_cast<WriteRequest*>(m_ptrCurrentWriteRequest.get());
    
	std::unique_lock<std::mutex> lock(m_writeMutex);
    
    if (ptrCurrentWriteRequest->GetStatus() != IORequest::PROCESSING) {
        /* Canceled, no further processing required. */
        //Handle write abort case;
        SetWriteActivity(false);
        return;
    }

    //user submitted Write request for 1000 bytes
    // but able to transfer only 250 bytes and some error occured 
    // 
    // if no error, this method will try to submit write job one more time
    // if 
    // m_writeSize = 1000 , transfer_size = 250
    // m_writeSize = 1000-250 = 750;
    // transferedBytes = 1000 - 750 = 250;

    m_writeSize -= transfer_size;
	size_t transferedBytes = ptrCurrentWriteRequest->GetBufferLength() - m_writeSize;

	IOResult result;
    if (m_bclosed) {
		result = CLOSED;
    }else if(error) {
        //GetSystemError()
        result = MapError(error);
    } else if ( m_writeSize > 0) { //still some data is remaining
        /* Incomplete write, schedule the rest. */
        memset(&m_writeCB, 0, sizeof(m_writeCB));
        if (m_writeOffset != FileStream::OFFSET_NONE) {
            m_writeOffset += transfer_size;
        }
        
        if (m_writeOffset == FileStream::OFFSET_NONE) {
            m_writeCB.Offset = 0;
            m_writeCB.OffsetHigh = 0;
        } else {
            m_writeCB.Offset = (DWORD)m_writeOffset;
			//m_writeCB.OffsetHigh = 0;
            m_writeCB.OffsetHigh = (DWORD)(m_writeOffset >> sizeof(m_writeCB.Offset) * 8);
        }
        const void *buf =
            reinterpret_cast<const UINT8 *>(ptrCurrentWriteRequest->GetBuffer()) + transferedBytes;
        if (!WriteFile(m_hFileWriteHandle == INVALID_HANDLE_VALUE ? m_hFileHandle : m_hFileWriteHandle,
                       buf, m_writeSize, NULL, &m_writeCB)
			) {

				DWORD error = ::GetLastError();
				if (error != ERROR_IO_PENDING && error != ERROR_MORE_DATA)
				{
					result = MapError(error);
					std::cout << "ERROR: WriteFile failed (continuation): " << GetSystemError().c_str();
				}
				else {
					/* Write for the rest data is scheduled. */
					return;
				}
        } 
    } else {
        /* Operation successfully completed. */
        result = OK;
    }
    //inform IOResult with number of bytes transfered.
	SetWriteActivity(false);
	ptrCurrentWriteRequest->Complete(result,IORequest::Status::COMPLETED, transferedBytes);
    
}
void
FileHandle::IOCompleteCallback(OVERLAPPED *io_cb, size_t transfer_size,
                                     DWORD error)
{
    if (io_cb == &m_writeCB) {
        WriteCompleteCallback(transfer_size, error);
    } else if (io_cb == &m_readCB) {
        ReadCompleteCallback(transfer_size, error);
    } else {
        std::cout<< "ERROR Invalid I/O control block received";
    }
}

void
FileHandle::Write(IRequest::Ptr writerequest)
{
    //std::cout<<"DEBUG::FileHandle::Write enter \n";
    WriteRequest* write_request = dynamic_cast<WriteRequest*>(writerequest.get());
    if(write_request == NULL) return;

	size_t writeSize = write_request->GetBufferLength();
	if (writeSize <= 0)
	{
		// A request to write 0 bytes on a handle is a no-op.
		write_request->Complete(IOResult::OK,IORequest::Status::COMPLETED, writeSize);
		return;
	}

    std::unique_lock<std::mutex> lock(m_writeMutex);
	m_writeSize = writeSize;
	

	//Check buffer address is sector aligned; -> if not allocate and write
	// check writeSize is Sector size?? -> if not write only sector size data.
	//check offset is sector aligned;

    std::memset(&m_writeCB, 0, sizeof(m_writeCB));

    m_writeOffset = write_request->GetOffset();

    if (m_writeOffset == FileStream::OFFSET_NONE) {
        m_writeCB.Offset = 0;
        m_writeCB.OffsetHigh = 0;
    } else {
		// Write the data. 
		DWORD offset = (DWORD)m_writeOffset;
		m_writeCB.Offset = offset & 0xFFFFFFFF;
		//m_writeCB.OffsetHigh = 0;
		m_writeCB.OffsetHigh = (DWORD)(m_writeOffset >> sizeof(m_writeCB.Offset) * 8);
		//m_writeCB.OffsetHigh = (offset >> 32) & 0xFFFFFFFF;
    }

    SetWriteActivity(true);

    write_request->SetStatus(IORequest::PROCESSING);
	m_ptrCurrentWriteRequest = writerequest;
	
	DWORD transferedBytes = 0;
	BOOL ok = WriteFile(m_hFileWriteHandle == INVALID_HANDLE_VALUE ? m_hFileHandle : m_hFileWriteHandle,
		write_request->GetBuffer(),
		writeSize, &transferedBytes, &m_writeCB);

	if (ok == FALSE) // write file failed??
	{
		DWORD error = ::GetLastError();
		if (error != ERROR_IO_PENDING && error != ERROR_MORE_DATA)
		{
			std::cout << "ERROR : FileHandle::Write failed:" << error;
			SetWriteActivity(false);
			write_request->Complete(MapError(error),IORequest::Status::COMPLETED, transferedBytes);
		}
		
	}
	//std::cout << "DEBUG:FileHandle::Write exit \n";
	return;
   
}