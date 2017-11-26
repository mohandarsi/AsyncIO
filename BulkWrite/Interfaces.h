#pragma once
#include "stdafx.h"

#ifdef BULKWRITE_EXPORTS
#define BULKWRITE_API __declspec(dllexport)
#else
#define BULKWRITE_API __declspec(dllimport)
#endif

#ifndef PURE
# define PURE	=0 //for pure virtual functions
#endif

#ifndef interface
# define interface	struct //c,c++ does n't have keyword for interface
#endif

#define DEFAULT_DESTRUCTOR(type) virtual ~##type##() = default;

/** Result of I/O operation. */
enum  IOResult {

    /** Operation completed successfully. */
    OK,
    /** Operation timed out. */
    TIMED_OUT,
    /** Operation canceled. */
    CANCELED,
    /** Bad address specific. It could not be understood and used. */
    BAD_ADDRESS,
    /** Remote side has explicitly refused the connection. */
    CONNECTION_REFUSED,
    /** Stream has been or is closed. All pending or new operations
     * initiated for closed stream are completed with this result. */
    CLOSED,
    /** Insufficient permissions for the requested operation. */
    PERMISSION_DENIED,
    /** End of file encountered. */
    END_OF_FILE,
    /** File locking error. Possible double lock or unlock while not locked*/
    LOCK_ERROR,
    /** Some other system failure. If happened, it is recommended to
     * investigate the root cause. */
    OTHER_FAILURE
};


interface BULKWRITE_API IRequest
{

    typedef std::shared_ptr<IRequest> Ptr;
     /* Wait on the operation to complete*/
    virtual bool Wait() PURE;
    virtual IOResult  GetIOResult(void)const PURE;
    virtual size_t GetTransferedBytes(void) const PURE ;

	DEFAULT_DESTRUCTOR(IRequest)
};


interface BULKWRITE_API IFileStream 
{
     /* Offset for read/write operations*/
    typedef __int64 Offset;

	//reference counted shard ptr
    typedef std::shared_ptr<IFileStream> Ptr;

    /** Default prototype for write operation completion handler. */
    typedef   void    (*WriteHandler) (IOResult , size_t len);

    virtual IRequest::Ptr
    Write(const void *data, size_t len, Offset offset, WriteHandler completion_handler) PURE;

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
    virtual IFileStream::Ptr
    Open(const std::string &name,const std::string &mode) PURE;

	/*
	   Enable the Proceesor to start the processing
	*/
    virtual void Enable() PURE;

	DEFAULT_DESTRUCTOR(IFileProcessor)
};

#ifdef __cplusplus
 # define EXTERN_C     extern "C"
#else
 #define EXTERN_C
#endif // __cplusplus


EXTERN_C  BULKWRITE_API IFileProcessor*  CreateFileProcessor();