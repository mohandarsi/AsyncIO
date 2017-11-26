#pragma once

#include "interfaces.h"

/** Generic request for implementing asynchronous  operations.  */
class IORequest : public IRequest
{
 public:
        
           /** Request processing status */
        enum Status {
            /** Request is currently pending for processing.       */
            PENDING,
            CANCELLATION_PENDING,
            CANCELING,
            PROCESSING,
            ABORT_PENDING,
            ABORTED,
            /** Request successfully processed. */
            COMPLETED,
            /** Request was canceled. */
            CANCELED
        };

        IORequest(const IFileStream::Offset offset):m_offset(offset){
           m_status =  PENDING ;
           m_bCompletionProcessed = false;
           m_transferedBytes = 0;
        }

        virtual
        ~IORequest(){}


        /** Get I/O operation offset value. */
        IFileStream::Offset  GetOffset()
        {
            return m_offset;
        }

        /** Get request current status. */
        Status
        GetStatus() const
        {
            return m_status;
        }

        size_t GetTransferedBytes() const override
        {
            return m_transferedBytes;
        }

        /** Complete the request processing.*/
        void
        Complete(IOResult ioResult,Status status = COMPLETED, size_t transferedBytes = 0);

        /* Wait on the operation to complete*/
		virtual void Wait() override;
		virtual bool Wait(std::chrono::seconds seconds) override ;

        /** Check if request is fully processed, i.e. all handlers were invoked and
         * no more actions pending.
         */
        bool
        IsCompleted() const
        {
            return m_bCompletionProcessed;
        }

        /** Returns the current request Result
        */
        IOResult
        GetIOResult() const override
        {
            return m_IOResult;
        }

protected:
        void SetStatus(Status status) 
        {
            m_status = status;
        }

        void SetTransferedBytes(size_t transferedBytes)
        {
              m_transferedBytes = transferedBytes;
        }

        void SetIOResult(IOResult ioResult)
        {
            m_IOResult =  ioResult;
        }

       virtual void HandleCompletion() PURE;

protected:

         /** I/O operation offset if necessary. */
        IFileStream::Offset m_offset;

        std::atomic<Status> m_status ;

        /** Mutex for protecting state modifications. */
        mutable std::mutex m_mutex;

        /** Condition variable for request state changes. */
		std::condition_variable m_condVar;

        /**  I/O result of the operation. */
        IOResult m_IOResult;

        /** Was the Complete() method invoked. */
        std::atomic<bool> m_bCompletionProcessed ;

        size_t m_transferedBytes;
};

class WriteRequest : public IORequest
{
   
	
public:
	typedef std::function<void (IOResult result, size_t size)> WriteCompletionHandlerType;
    
    friend class FileHandle;

    WriteRequest(const IFileStream::Offset offset,const void* buffer,const size_t len):IORequest(offset),
      m_buffer(buffer),m_bufferLen(len)
    {}

   	void AddCompletionHandler(WriteCompletionHandlerType  handler)
	{
		m_Completionhandlers.push_back(handler);
		//m_handler = handler;
	}

   const void* 
    GetBuffer() { return m_buffer;}

    size_t 
    GetBufferLength(){return m_bufferLen;}

    virtual ~WriteRequest(){
		//std::cout << " \n DEBUG: Destructor WriteRequest \n";
    }
protected:
    virtual void HandleCompletion() override;

private:
    /** Buffer with data to write.  */
    const void* m_buffer;
    size_t m_bufferLen;
	std::vector<WriteCompletionHandlerType> m_Completionhandlers;
};