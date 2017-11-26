#include "IORequest.h"
#include "FileStream.h"


void
IORequest::Complete(IOResult ioResult,Status status, size_t transferedBytes)
{
	
	if (m_bCompletionProcessed) {
		new std::exception("Request is already completed");
	}

	if (status < PENDING) {
		new std::exception("Disallowed status value specified");
	}
    //std::cout<<"DEBUG:IORequest::Complete \n";
    //std::unique_lock<std::mutex> lock(m_mutex);

	m_IOResult = ioResult;
    this->m_status = status;
	m_transferedBytes = transferedBytes;

    //call completion handlers
    HandleCompletion();

   /* Submit to target completion context only if completion context specified. */
    m_bCompletionProcessed = true;
    m_condVar.notify_all();

}

void
IORequest::Wait()
{
    //std::cout<<"DEBUG:IORequest::Wait enter \n";
    std::unique_lock<std::mutex> lock(m_mutex);
	m_condVar.wait(lock, [&] { return IsCompleted(); });
    //std::cout<<"\n DEBUG:IORequest::Wait exit \n";
}

bool
IORequest::Wait(std::chrono::seconds seconds)
{
	//std::cout<<"DEBUG:IORequest::Wait enter \n";
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_condVar.wait_for(lock, seconds,[&] { return IsCompleted(); });
	//std::cout<<"\n DEBUG:IORequest::Wait exit \n";
}

void WriteRequest::HandleCompletion()
{
	for each (auto handler in m_Completionhandlers)
	{
		handler(m_IOResult, m_transferedBytes);
	}
   // m_Completionhandlers(m_IOResult,m_transferedBytes);
}

//void WriteRequest::Cancel()
//{
//	m_status = CANCELING;
//
//	m_status = CANCELED;
//}

