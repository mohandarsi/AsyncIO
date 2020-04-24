#pragma once
#include "IORequest.h"
class WriteIORequest : public IORequest
{

public:
	typedef std::function<void(Status result, size_t size)> WriteCompletionHandlerType;

	

	WriteIORequest(const Offset offset, const void* buffer, const size_t len) :IORequest(offset, buffer,len)
		{}

	void AddCompletionHandler(WriteCompletionHandlerType  handler)
	{
		m_Completionhandlers.push_back(handler);
	}

	virtual ~WriteIORequest() {
		//std::cout << " \n DEBUG: Destructor WriteRequest \n";
	}
protected:
	virtual void HandleCompletion() override;

private:
	
	std::vector<WriteCompletionHandlerType> m_Completionhandlers;
};

