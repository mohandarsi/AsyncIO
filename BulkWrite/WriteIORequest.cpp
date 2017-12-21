#include "WriteIORequest.h"



void WriteIORequest::HandleCompletion()
{
	for each (auto handler in m_Completionhandlers)
	{
		handler(m_IOResult, m_transferedBytes);
	}
	// m_Completionhandlers(m_IOResult,m_transferedBytes);
}
