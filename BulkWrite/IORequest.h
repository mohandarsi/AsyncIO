#pragma once

#include "interfaces.h"

/** Generic request for implementing asynchronous  operations.  */
class IORequest : public IRequest
{
	friend class FileHandle;
 public:

     IORequest(const IFileStream::Offset offset,const void* buffer, const size_t len):m_offset(offset),m_buffer(buffer), m_bufferLen(len) {
           m_bCompletionProcessed = false;
           m_transferedBytes = 0;
        }

        virtual
        ~IORequest(){}


        /** Get I/O operation offset value. */
        IFileStream::Offset  GetOffset() const
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
        Complete(Status status = Status::COMPLETED, size_t transferedBytes = 0);

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
        Status
        GetIOStatus() const override
        {
            return m_status;
        }

		const void*
			GetBuffer() { return m_buffer; }

		size_t
			GetBufferLength() const { return m_bufferLen; }
protected:
        void SetStatus(Status status) 
        {
            m_status = status;
        }

        void SetTransferedBytes(size_t transferedBytes)
        {
              m_transferedBytes = transferedBytes;
        }

       virtual void HandleCompletion() PURE;

protected:
		/** Buffer with data to write/read.  */
		const void* m_buffer;
		
		size_t m_bufferLen;

         /** I/O operation offset if necessary. */
        IFileStream::Offset m_offset;

        std::atomic<Status> m_status ;
        /** Was the Complete() method invoked. */
        std::atomic<bool> m_bCompletionProcessed ;
        size_t m_transferedBytes;
};

