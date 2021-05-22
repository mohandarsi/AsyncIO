#pragma once

#include <memory>
#include <functional>

#if !defined(ASYNCFILEIO_API)

#ifdef ASYNCFILEIO_EXPORTS
#define ASYNCFILEIO_API __declspec(dllexport)
#else
#define ASYNCFILEIO_API __declspec(dllimport)
#endif

#endif

#pragma warning( push )
#pragma warning( disable : 4251 ) // TODO how to avoid it??.

namespace AsyncFileIO
{

class ASYNCFILEIO_API IOBuffer
{
public:

	explicit IOBuffer(size_t bufferSize);
	IOBuffer(void *data, size_t bufferSize, const std::function<void (void*)>& deleter);
	
	const void* data() const;
	void* data();
	size_t size() const;

private:
	std::shared_ptr<void> m_buffer;
	size_t m_size;
	
};
	
}

#pragma warning( pop )

