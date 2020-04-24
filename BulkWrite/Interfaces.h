#pragma once
#include<future>

#ifdef BULKWRITE_EXPORTS
#define BULKWRITE_API __declspec(dllexport)
#else
#define BULKWRITE_API __declspec(dllimport)
#endif

#include "Definitions.h"

/** Request processing status */


namespace FileAPI
{

interface BULKWRITE_API IRequest
{

    virtual Status  GetIOStatus(void)const PURE;
    virtual size_t  GetTransferedBytes(void) const PURE ;

	DEFAULT_DESTRUCTOR(IRequest)
};

interface BULKWRITE_API IFileStream 
{

    virtual std::future<IOStatus>
    Write(const void *data, size_t len, Offset offset) PURE;

	virtual void SetFileSize(size_t len) PURE;

	DEFAULT_DESTRUCTOR(IFileStream)
};

interface BULKWRITE_API IFileProcessor
{
    /*Open file.
	"r"	read: Open file for input operations. The file must exist.
	"r+"	read/update: Open a file for update (both for input and output). The file must exist.
	"w"	write: Create an empty file for output operations. 
	           If a file with the same name already exists,its contents are discarded and the file is treated as a new empty file.
	 w+"	write/update: Create an empty file and open it for update (both for input and output). 
	        If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
	
	*/
    virtual std::unique_ptr<IFileStream>
    Open(const std::string &name,const std::string &mode) PURE;

	/*
	   Enable the Proceesor to start the processing
	*/
    virtual void Enable() PURE;

	DEFAULT_DESTRUCTOR(IFileProcessor)
};

}
BULKWRITE_API std::unique_ptr<FileAPI::IFileProcessor>  CreateFileProcessor();