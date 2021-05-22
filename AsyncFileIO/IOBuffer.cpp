#include "IOBuffer.h"

namespace AsyncFileIO
{

//TODO move this to generic place and add some log.
#define LOGGING_CRASH ((void)(*(volatile char*)0 = 0))

#define EAT_STREAM_PARAMETERS ((void) 0)

#define CHECK(condition) \
  !(condition) ? LOGGING_CRASH : EAT_STREAM_PARAMETERS;
	
void AssertValidBufferSize(const size_t size) {
	CHECK(size > 0)
}
	
IOBuffer::IOBuffer(const size_t bufferSize)
{
	AssertValidBufferSize(bufferSize);
	m_buffer = std::shared_ptr<unsigned char>(new unsigned char[bufferSize], std::default_delete<unsigned char[]>());
	m_size = bufferSize;
}

IOBuffer::IOBuffer(void* data, size_t bufferSize, const std::function<void(void*)>& deleter)
{
	AssertValidBufferSize(bufferSize);
	m_buffer = std::shared_ptr<void>(data, [deleter, data](void*){
			if(deleter) (deleter)(data);
		});
	m_size = bufferSize;
}

void* IOBuffer::data()
{
	return m_buffer ? m_buffer.get() : nullptr;
}

const void* IOBuffer::data() const
{
	return m_buffer ? m_buffer.get() : nullptr;
}

size_t IOBuffer::size() const
{
	return m_size;
}
	
}
