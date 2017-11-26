#pragma once
#include "interfaces.h"

class FileHandle {
public:

    /** Construct an instance based on already opened handles.
        Used for Read scenario's
    */
    FileHandle(
            HANDLE handle,
            HANDLE write_handle = INVALID_HANDLE_VALUE);

    /** Construct an instance by opening a file specified by path. */
    FileHandle(
    		const std::string &path,
    		FileMode mode);

    /** Closes handles on destruction. */
    virtual
    ~FileHandle();

	virtual void SetFileSize(size_t size);

    /** Set associated stream. */
     void
     SetStream(IFileStream::Ptr stream);

    /** Schedule write operation based on current write request. */
    virtual void
    Write(IRequest::Ptr) ;

    /** Schedule read operation based on current read request. */
    virtual void
        Read() {}

    bool 
        IsClosed() { return m_bclosed;}

    /** Completion handler for platform write call.
     */
    void
    WriteCompleteCallback(size_t transfer_size, DWORD error);

    /** Completion handler for platform read call.
     */
    void
    ReadCompleteCallback(size_t transfer_size, DWORD error);

    /** Completion handler for platform I/O call.
     */
    void
    IOCompleteCallback(OVERLAPPED *io_cb, size_t transfer_size, DWORD error);

   

    /** Current handle Write Status. */
    bool GetWriteActivity();

private:

    /* indicate whether write call is
              currently active or not.*/
    void 
    SetWriteActivity(bool is_active);

	/** Close the handle. */
	void  Close();

private:
    struct WriteContext
    {

         /** Holds current WriteRequest ptr. */
        IRequest::Ptr m_ptrCurrentWriteRequest;
        
        /** Control block for current write operation. */
        OVERLAPPED m_writeCB;

         /** Number of bytes pending for write. */
        size_t m_writeSize;

         /** File offset for write operation. */
        IFileStream::Offset  m_writeOffset;
    };
    friend class OverlappedIOController;

    /** Opened file handle for reading/writing. */
    HANDLE m_hFileHandle;
    /** Use this handle for writing, if specified. */
    HANDLE m_hFileWriteHandle;

    /** Is the handle already closed. */
    bool m_bclosed;

    /** Related stream. */
    std::weak_ptr<IFileStream> m_ptrStream;

	IRequest::Ptr m_ptrCurrentWriteRequest;
   
    /** Holds reference to a stream while write operation is in progress. */
    IFileStream::Ptr m_ptrWriteActiveStream,
    /** Holds reference to a stream while read operation is in progress. */
                m_ptrReadActiveStream;

    /** Control block for current read operation. */
    OVERLAPPED m_readCB;
    /** Control block for current write operation. */
    OVERLAPPED m_writeCB;
    
    /** Read buffer. */
    std::shared_ptr<std::vector<UINT8>> read_buf;

    /** Number of bytes pending for read. */
    size_t read_size,
    /** Number of bytes pending for write. */
           m_writeSize,
   /** Minimal number of bytes to read in current read operation. */
           min_read_size;
   
    /** File offset for read operation. */
    IFileStream::Offset read_offset,
    /** File offset for write operation. */
           m_writeOffset;

    /** Mutex for protecting write control block. */
    std::mutex m_writeMutex,
    /** Mutex for protecting read control block. */
               read_mutex;
};