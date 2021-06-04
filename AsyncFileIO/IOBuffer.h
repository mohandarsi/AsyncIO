#pragma once

#include <functional>
#include  "dllexportimport.h"

namespace AsyncFileIO
{

class ASYNCFILEIO_API IOBuffer
{
public:

	explicit IOBuffer(size_t bufferSize);
	// deleter gets called when IOBuffer goes out if scope
	IOBuffer(void *data, size_t bufferSize, const std::function<void (void*)>& deleter);
	IOBuffer(const IOBuffer&) = delete;
	~IOBuffer();

	IOBuffer& operator=(const IOBuffer&) = delete;
	
	const void* data() const;
	void* data();
	size_t size() const;

private:
	struct Private;
	Private* m_p;
};
	
}

