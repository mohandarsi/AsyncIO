#pragma once
#include "IORequest.h"
class ReadIORequest :
	public IORequest, OVERLAPPED
{
public:
	ReadIORequest::ReadIORequest(const IFileStream::Offset offset, const void* buffer, const size_t len) : IORequest(offset, buffer, len){}
	virtual ~ReadIORequest();
private:
	
};

