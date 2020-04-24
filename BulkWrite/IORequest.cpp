#include "IORequest.h"
#include "FileStream.h"


void
IORequest::Complete(Status status, size_t transferedBytes)
{
	
	if (m_bCompletionProcessed) {
		new std::exception("Request is already completed");
	}
    //std::cout<<"DEBUG:IORequest::Complete \n";

    this->m_status = status;
	m_transferedBytes = transferedBytes;

    //call completion handlers
    HandleCompletion();

}


//void WriteRequest::Cancel()
//{
//	m_status = CANCELING;
//
//	m_status = CANCELED;
//}

