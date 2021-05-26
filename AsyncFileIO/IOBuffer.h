#pragma once

#include <functional>

#if !defined(ASYNCFILEIO_API)

#ifdef ASYNCFILEIO_EXPORTS
#define ASYNCFILEIO_API __declspec(dllexport)
#else
#define ASYNCFILEIO_API __declspec(dllimport)
#endif

#endif

namespace AsyncFileIO
{

class ASYNCFILEIO_API IOBuffer
{
public:

	explicit IOBuffer(size_t bufferSize);
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

