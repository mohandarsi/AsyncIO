#pragma once

#include<future>

#include "Definitions.h"

namespace FileAPI
{

class FileMode;

class FileHandle {
public:

    /** Construct an instance by opening a file specified by path. */
    FileHandle(
    		const std::string &path,
    		const FileMode& mode);

    /** Closes handles on destruction. */
    virtual
    ~FileHandle();

	virtual void SetFileSize(size_t size);

    /** Schedule write operation based on current write request. */
    std::future<IOStatus>  Write(const Offset offset, const void* buffer, const size_t len) ;
	
    bool 
        IsClosed() { return m_bclosed;}

private:

    /** Close the handle. */
	void  Close();

private:
  

    friend class OverlappedIOController;

    /** Opened file handle for reading/writing. */
    std::shared_ptr<void> m_hFileHandle;

    /** Is the handle already closed. */
    bool m_bclosed;

};

}